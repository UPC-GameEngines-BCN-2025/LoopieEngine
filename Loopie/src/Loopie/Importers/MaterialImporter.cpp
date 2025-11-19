#include "MaterialImporter.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Files/Json.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem> // Used for checking the extension
#include <cstddef> // Used for size_t

namespace Loopie {

	static std::vector<float> ParseNumberList(const std::string& s) {
		std::vector<float> out;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, ',')) {
			out.push_back(std::stof(item));
		}
		return out;
	}

	void MaterialImporter::ImportMaterial(const std::string& filepath, Metadata& metadata) {
		if (metadata.HasCache)
			return;

		JsonData jsonData = Json::ReadFromFile(filepath);
		if (jsonData.IsEmpty())
			return;

		JsonResult<std::string> shaderNode = jsonData.GetValue<std::string>("shader");
		std::string shaderUUID = shaderNode.Result;

		JsonNode propertiesNode = jsonData.Child("properties");
		std::vector<std::string> propKeys = propertiesNode.GetObjectKeys();

		Project project = Application::GetInstance().m_activeProject;
		UUID id;
		std::filesystem::path locationPath = "Materials";
		locationPath /= id.Get() + ".material";

		std::filesystem::path pathToWrite = project.GetChachePath() / locationPath;

		std::ofstream fs(pathToWrite, std::ios::binary | std::ios::trunc);
		

		fs.write(shaderUUID.c_str(), UUID::UUID_SIZE);

		unsigned int propertyCount = (unsigned int)propKeys.size();
		fs.write(reinterpret_cast<const char*>(&propertyCount), sizeof(propertyCount));

		for (const auto& key : propKeys)
		{
			JsonNode propNode = propertiesNode.Child(key);
			JsonResult<std::string> typeResult = propNode.GetValue<std::string>("type");
			JsonResult<std::string> valueResult = propNode.GetValue<std::string>("value");

			if (!typeResult.Found || !valueResult.Found)
				continue;
			const std::string& type = typeResult.Result;
			const std::string& value = valueResult.Result;

			unsigned int nameLength = (unsigned int)key.size();
			fs.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
			fs.write(key.c_str(), nameLength);

			///Write EnumType
			///Write Value

			unsigned int enumLength = (unsigned int)key.size();
			fs.write(reinterpret_cast<const char*>(&enumLength), sizeof(enumLength));
			if (type == "Int") {
				int dataValue = std::stoi(value);
				fs.write(reinterpret_cast<const char*>(&dataValue), sizeof(dataValue));
			}
			else if (type == "Float") {
				float dataValue = std::stof(value);
				fs.write(reinterpret_cast<const char*>(&dataValue), sizeof(dataValue));
			}
			else if (type == "UInt") {
				unsigned int dataValue = std::stoul(value);
				fs.write(reinterpret_cast<const char*>(&dataValue), sizeof(dataValue));
			}
			else if (type == "Bool") {
				int dataValue = value == "True" ? 1 : 0;
				fs.write(reinterpret_cast<const char*>(&dataValue), sizeof(dataValue));
			}
			else if (type == "Vec2" || type == "Vec3" || type == "Vec4" || type == "Mat2" || type == "Mat3" || type == "Mat4") {		
				auto nums = ParseNumberList(value);
				for(const auto& dataValue : nums)
					fs.write(reinterpret_cast<const char*>(&dataValue), sizeof(dataValue));
			}
			else if (type == "Sampler2D") {
				//fs.write(shaderUUID.c_str(), UUID::UUID_SIZE);
			}
			else if (type == "Sampler3D") {
				//fs.write(shaderUUID.c_str(), UUID::UUID_SIZE);
			}
			else if (type == "SamplerCube") {
				//fs.write(shaderUUID.c_str(), UUID::UUID_SIZE);
			}
		}

		fs.close();

		metadata.HasCache = true;
		metadata.cachesPath.clear();
		metadata.cachesPath.push_back(locationPath.string());
		MetadataRegistry::SaveMetadata(filepath, metadata);

	}

	void MaterialImporter::LoadMaterial(const std::string& path, Material& material) {
		Project project = Application::GetInstance().m_activeProject;
		std::filesystem::path filepath = project.GetChachePath() / path;
		if (!std::filesystem::exists(filepath))
			return;

		/// READ
		std::ifstream file(filepath, std::ios::binary);
		if (!file) {
			Log::Warn("Error opening .material file -> {0}", filepath.string());
			return;
		}

		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		if (size <= 0) {
			Log::Warn("Error reading .material file -> {0}", filepath.string());
			return;
		}

		///Load

		std::string id;
		file.read(id.data(), UUID::UUID_SIZE);
		UUID shaderUUID = UUID(id);

		unsigned int propertiesCount = 0;
		file.read(reinterpret_cast<char*>(&propertiesCount), sizeof propertiesCount);

		for (size_t i = 0; i < propertiesCount; i++)
		{
			unsigned int propertyNameLength = 0;
			file.read(reinterpret_cast<char*>(&propertyNameLength), sizeof propertyNameLength);

			std::string propertyName(propertyNameLength, '\0');
			file.read(propertyName.data(), sizeof propertyNameLength);

			//// ReadEnume
			unsigned int typeLength = 0;
			file.read(reinterpret_cast<char*>(&typeLength), sizeof(typeLength));
			std::string type(typeLength, '\0');
			file.read(&type[0], typeLength);


			UniformValue uv{};

			if (type == "Int") {
				int v; 
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_int; uv.value = v;
			}
			else if (type == "Float") {
				float v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_float; uv.value = v;
			}
			else if (type == "UInt") {
				unsigned int v; 
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_uint; uv.value = v;
			}
			else if (type == "Bool") {
				bool b; 
				file.read(reinterpret_cast<char*>(&b), sizeof(b));
				uv.type = UniformType_bool; uv.value = (b != 0);
			}
			else if (type == "Vec2") {
				glm::vec2 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_vec2;
			}
			else if (type == "Vec3") {
				glm::vec3 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_vec3;
			}
			else if (type == "Vec4") {
				glm::vec4 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_vec4;
			}
			else if (type == "Mat2") {
				glm::mat2 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_mat2;
			}
			else if (type == "Mat3") {
				glm::mat3 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_mat3;
			}
			else if (type == "Mat4") {
				glm::mat4 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_mat4;
			}
			else if (type == "Sampler2D" ) {

			}
			else if (type == "Sampler3D") {

			}
			else if (type == "SamplerCube") {

			}

			material.SetShaderVariable(propertyName, uv);
		}

		file.close();
	}

	bool MaterialImporter::CheckIfIsMaterial(const char* path) {
		std::string extension = std::filesystem::path(path).extension().string();
		for (char& c : extension)
		{
			c = std::tolower(static_cast<unsigned char>(c));
		}

		if (!extension.empty() && extension[0] == '.')
			extension = extension.substr(1);

		return extension == "mat";
	}
}