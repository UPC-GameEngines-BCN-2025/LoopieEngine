#include "AssetRegistry.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Files/DirectoryManager.h"


#include "Loopie/Importers/TextureImporter.h"
#include "Loopie/Importers/MeshImporter.h"

#include <filesystem>

namespace Loopie {

	std::unordered_map<UUID, Metadata> AssetRegistry::s_Assets;
	std::unordered_map<std::string, UUID> AssetRegistry::s_PathToUUID;
	std::unordered_map<UUID, std::string> AssetRegistry::s_UUIDToPath;

	void AssetRegistry::Initialize() {
	
		RefreshAssetRegistry();

		Log::Info("AssetRegistry initialized, {} assets found", s_Assets.size());
	}

	void AssetRegistry::Shutdown() {
		Clear();
	}

	void AssetRegistry::RefreshAssetRegistry() {
		Clear();
		CleanOrphanedMetadata();
		ScanEngineDirectory();
		ScanAssetDirectory();

		for (auto& [key, metadata] : s_Assets) {
			
			if (metadata.IsOutdated) {
				const std::string& pathString = s_UUIDToPath[metadata.uuid];
				metadata.LastModified = MetadataRegistry::GetLastModifiedFromPath(pathString);
				Log::Info("{0}", pathString);
				/// DO REIMPORTS

				if (TextureImporter::CheckIfIsImage(pathString.c_str())) {
					TextureImporter::ImportImage(pathString, metadata);
				}
				else if (MeshImporter::CheckIfIsModel(pathString.c_str())) {
					MeshImporter::ImportModel(pathString, metadata);
				}

				///
				UpdateMetadata(metadata, pathString);
			}
		}
	}

	void AssetRegistry::Clear() {
		s_Assets.clear();
		s_PathToUUID.clear();
		s_UUIDToPath.clear();
	}
	void AssetRegistry::ScanEngineDirectory() {
		for (auto& entry : std::filesystem::recursive_directory_iterator("assets"))
		{
			auto path = entry.path();
			if (path.extension() == ".meta")
				continue;

			Metadata metadata = MetadataRegistry::GetMetadataAsset(path.string());
			Register(path.string(), metadata);
		}
	}

	void AssetRegistry::ScanAssetDirectory()
	{
		const Project& project = Application::GetInstance().m_activeProject;
		for (auto& entry : std::filesystem::recursive_directory_iterator(project.GetAssetsPath()))
		{
			auto path = entry.path();
			if (path.extension() == ".meta")
				continue;

			Metadata metadata = MetadataRegistry::GetMetadataAsset(path.string());
			Register(path.string(), metadata);
		}
	}

	void AssetRegistry::CleanOrphanedMetadata()
	{
		const Project& project = Application::GetInstance().m_activeProject;
		for (auto& entry : std::filesystem::recursive_directory_iterator(project.GetAssetsPath()))
		{
			if (MetadataRegistry::IsMetadataFile(entry.path()))
			{
				std::filesystem::path sourceFile = entry.path();
				sourceFile.replace_extension("");

				if (!std::filesystem::exists(sourceFile))
				{
					std::filesystem::remove(entry.path());
					///Remove cache files also?
				}
			}
		}
	}

	Metadata& AssetRegistry::GetOrCreateMetadata(const std::filesystem::path& assetPath)
	{
		std::string pathStr = assetPath.string();

		if (s_PathToUUID.count(pathStr))
			return s_Assets[s_PathToUUID[pathStr]];

		Metadata metadata = MetadataRegistry::GetMetadataAsset(pathStr);
		Register(pathStr, metadata);

		return s_Assets[metadata.uuid];
	}

	Metadata* AssetRegistry::GetMetadata(const UUID& uuid)
	{
		auto it = s_Assets.find(uuid);
		if (it == s_Assets.end())
			return nullptr;
		return &it->second;
	}

	Metadata* AssetRegistry::GetMetadata(const std::string& sourcePath)
	{
		auto it = s_PathToUUID.find(sourcePath);
		if (it == s_PathToUUID.end())
			return nullptr;

		return GetMetadata(it->second);
	}

	bool AssetRegistry::UpdateMetadata(const Metadata& metadata, const std::filesystem::path& assetPath)
	{
		Register(assetPath.string(), metadata);
		MetadataRegistry::SaveMetadata(assetPath, metadata);
		return true;
	}

	void AssetRegistry::Register(const std::string& path, const Metadata& metadata)
	{
		s_Assets[metadata.uuid] = metadata;
		s_PathToUUID[path] = metadata.uuid;
		s_UUIDToPath[metadata.uuid] = path;
	}
}