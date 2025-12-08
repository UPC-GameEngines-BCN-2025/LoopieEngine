#include "TextureImporter.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#include <IL/il.h>
#include <IL/ilu.h>
#include <lz4.h>

namespace Loopie {

    void TextureImporter::ImportImage(const std::string& filepath, Metadata& metadata)
    {
        if (metadata.HasCache && !metadata.IsOutdated)
            return;

        ILuint imageID;
        ilGenImages(1, &imageID);
        ilBindImage(imageID);

        if (!ilLoadImage(filepath.c_str())) {
            Log::Error("Failed to load image {0}", filepath);
            ilDeleteImages(1, &imageID);
            return;
        }

        ILint format = ilGetInteger(IL_IMAGE_FORMAT);
        ILint type = ilGetInteger(IL_IMAGE_TYPE);
        if (format != IL_RGBA || type != IL_UNSIGNED_BYTE) {
            if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
                Log::Error("Failed to convert image {0}", filepath);
                ilDeleteImages(1, &imageID);
                return;
            }
        }

        int width = ilGetInteger(IL_IMAGE_WIDTH);
        int height = ilGetInteger(IL_IMAGE_HEIGHT);
        int channels = ilGetInteger(IL_IMAGE_CHANNELS);
        unsigned int imageSize = static_cast<unsigned int>(width) * height * channels;

        ILubyte* data = ilGetData();
        if (!data) {
            Log::Error("Invalid image data for {0}", filepath);
            ilDeleteImages(1, &imageID);
            return;
        }

        int maxCompressedSize = LZ4_compressBound(static_cast<int>(imageSize));
        std::vector<char> compressedBuffer;
        compressedBuffer.resize(maxCompressedSize);

        int compressedSize = LZ4_compress_default(reinterpret_cast<const char*>(data), compressedBuffer.data(), static_cast<int>(imageSize), maxCompressedSize);

        if (compressedSize <= 0) {
            Log::Error("Failed to compress image {0}", filepath);
            ilDeleteImages(1, &imageID);
            return;
        }
        ilDeleteImages(1, &imageID);

        Project project = Application::GetInstance().m_activeProject;
        UUID id;
        std::filesystem::path locationPath = "Textures";
        locationPath /= id.Get() + ".texture";
        std::filesystem::path pathToWrite = project.GetChachePath() / locationPath;

        std::ofstream fs(pathToWrite, std::ios::binary | std::ios::trunc);
        if (!fs) {
            Log::Error("Failed to create cache file for {0}", filepath);
            return;
        }

        fs.write(reinterpret_cast<const char*>(&width), sizeof(width));
        fs.write(reinterpret_cast<const char*>(&height), sizeof(height));
        fs.write(reinterpret_cast<const char*>(&channels), sizeof(channels));
        fs.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));
        fs.write(compressedBuffer.data(), compressedSize);
        fs.close();


        compressedBuffer.clear();
        compressedBuffer.shrink_to_fit();

        metadata.HasCache = true;
        metadata.CachesPath.clear();
        metadata.CachesPath.push_back(locationPath.string());
        metadata.Type = ResourceType::TEXTURE;

        MetadataRegistry::SaveMetadata(filepath, metadata);

        Log::Trace("Texture Imported -> {0} (Compressed: {1:.2f}MB -> {2:.2f}MB, Ratio: {3:.1f}%)", filepath, imageSize / (1024.0 * 1024.0), compressedSize / (1024.0 * 1024.0), (compressedSize * 100.0) / imageSize);
    }


    void TextureImporter::LoadImage(const std::string& path, Texture& texture)
    {
        Project project = Application::GetInstance().m_activeProject;
        std::filesystem::path filepath = project.GetChachePath() / path;

        if (!std::filesystem::exists(filepath)) {
            Log::Warn("Texture cache file not found: {0}", filepath.string());
            return;
        }

        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            Log::Warn("Error opening .texture file -> {0}", filepath.string());
            return;
        }

        file.read(reinterpret_cast<char*>(&texture.m_width), sizeof(texture.m_width));
        file.read(reinterpret_cast<char*>(&texture.m_height), sizeof(texture.m_height));
        file.read(reinterpret_cast<char*>(&texture.m_channels), sizeof(texture.m_channels));

        int compressedSize = 0;
        file.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));

        if (texture.m_width <= 0 || texture.m_height <= 0 || texture.m_channels <= 0 || compressedSize <= 0) {
            Log::Warn("Invalid texture data in file -> {0}", filepath.string());
            return;
        }

        std::vector<char> compressedData(compressedSize);
        file.read(compressedData.data(), compressedSize);
        file.close();

        unsigned int imageSize = static_cast<unsigned int>(texture.m_width) * texture.m_height * texture.m_channels;
        std::vector<unsigned char> decompressedData(imageSize);

        int decompressedSize = LZ4_decompress_safe(compressedData.data(), reinterpret_cast<char*>(decompressedData.data()), compressedSize, static_cast<int>(imageSize));

        if (decompressedSize < 0) {
            Log::Error("Failed to decompress texture: {0}", path);
            return;
        }

        texture.m_tb = std::make_shared<TextureBuffer>(decompressedData.data(), texture.m_width, texture.m_height, texture.m_channels);

        decompressedData.clear();
        decompressedData.shrink_to_fit();
        compressedData.clear();
        compressedData.shrink_to_fit();

        Log::Trace("Texture uploaded to GPU -> {0} ({1}x{2}), RAM freed", path, texture.m_width, texture.m_height);
    }

    bool TextureImporter::CheckIfIsImage(const char* path)
    {
        ILenum type = ilDetermineType(path);
        return type != IL_TYPE_UNKNOWN;
    }
}