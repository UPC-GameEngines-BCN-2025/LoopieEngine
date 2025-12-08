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
		if (metadata.HasCache && !metadata.IsOutdated)
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

			unsigned int typeLenght = (unsigned int)type.size();
			fs.write(reinterpret_cast<const char*>(&typeLenght), sizeof(typeLenght));
			fs.write(type.c_str(), typeLenght);

			///Write Value
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
		metadata.CachesPath.clear();
		metadata.CachesPath.push_back(locationPath.string());
		metadata.Type = ResourceType::MATERIAL;

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

		std::string id(UUID::UUID_SIZE, '\0');
		file.read(id.data(), UUID::UUID_SIZE);
		//UUID shaderUUID = UUID(id);

		unsigned int propertiesCount = 0;
		file.read(reinterpret_cast<char*>(&propertiesCount), sizeof(propertiesCount));

		for (size_t i = 0; i < propertiesCount; i++)
		{
			unsigned int propertyNameLength = 0;
			file.read(reinterpret_cast<char*>(&propertyNameLength), sizeof(propertyNameLength));
			std::string propertyName(propertyNameLength, '\0');
			file.read(propertyName.data(), propertyNameLength);

			unsigned int propertyTypeLength = 0;
			file.read(reinterpret_cast<char*>(&propertyTypeLength), sizeof(propertyTypeLength));
			std::string propertyType(propertyTypeLength, '\0');
			file.read(propertyType.data(), propertyTypeLength);



			UniformValue uv{};

			if (propertyType == "Int") {
				int v; 
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_int; 
				uv.value = v;
			}
			else if (propertyType == "Float") {
				float v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_float; 
				uv.value = v;
			}
			else if (propertyType == "UInt") {
				unsigned int v; 
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_uint; 
				uv.value = v;
			}
			else if (propertyType == "Bool") {
				bool b; 
				file.read(reinterpret_cast<char*>(&b), sizeof(b));
				uv.type = UniformType_bool; 
				uv.value = (b != 0);
			}
			else if (propertyType == "Vec2") {
				glm::vec2 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_vec2;
				uv.value = v;
			}
			else if (propertyType == "Vec3") {
				glm::vec3 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_vec3;
				uv.value = v;
			}
			else if (propertyType == "Vec4") {
				glm::vec4 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_vec4;
				uv.value = v;
			}
			else if (propertyType == "Mat2") {
				glm::mat2 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_mat2;
				uv.value = v;
			}
			else if (propertyType == "Mat3") {
				glm::mat3 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_mat3;
				uv.value = v;
			}
			else if (propertyType == "Mat4") {
				glm::mat4 v;
				file.read(reinterpret_cast<char*>(&v), sizeof(v));
				uv.type = UniformType_mat4;
				uv.value = v;
			}
			else if (propertyType == "Sampler2D" ) {
				uv.type = UniformType_Sampler2D;
			}
			else if (propertyType == "Sampler3D") {
				uv.type = UniformType_Sampler3D;
			}
			else if (propertyType == "SamplerCube") {
				uv.type = UniformType_Sampler3D;
			}

			material.SetShaderVariable(propertyName, uv);
		}

		file.close();
	}

	void MaterialImporter::SaveMaterial(const std::string& filepath, Material& material, Metadata& metadata)
	{
		JsonData jsonData;

		Shader& shader = material.GetShader();
		UUID randomUUID;
		//std::string shaderUUIDString = shader.GetUUID().Get();

		jsonData.CreateField("shader", randomUUID.Get());
		JsonNode propertiesNode = jsonData.CreateObjectField("properties");

		const auto& props = material.GetUniforms();
		for (const auto& [id, uniformValue] : props)
		{
			std::string typeString;
			std::string valueString;

			switch (uniformValue.type)
			{
			case UniformType_int:
			{
				typeString = "Int";
				int v = std::get<int>(uniformValue.value);
				valueString = std::to_string(v);
				break;
			}
			case UniformType_float:
			{
				typeString = "Float";
				float v = std::get<float>(uniformValue.value);
				valueString = std::to_string(v);
				break;
			}
			case UniformType_uint:
			{
				typeString = "UInt";
				unsigned int v = std::get<unsigned int>(uniformValue.value);
				valueString = std::to_string(v);
				break;
			}
			case UniformType_bool:
			{
				typeString = "Bool";
				bool v = std::get<bool>(uniformValue.value);
				valueString = v ? "True" : "False";
				break;
			}
			case UniformType_vec2:
			{
				typeString = "Vec2";
				auto v = std::get<glm::vec2>(uniformValue.value);
				valueString = GLMVectorToString(v);
				break;
			}
			case UniformType_vec3:
			{
				typeString = "Vec3";
				auto v = std::get<glm::vec3>(uniformValue.value);
				valueString = GLMVectorToString(v);
				break;
			}
			case UniformType_vec4:
			{
				typeString = "Vec4";
				auto v = std::get<glm::vec4>(uniformValue.value);
				valueString = GLMVectorToString(v);

				break;
			}
			case UniformType_mat2:
			{
				typeString = "Mat2";
				auto m = std::get<glm::mat2>(uniformValue.value);
				valueString = GLMMatrixToString(m);
				break;
			}

			case UniformType_mat3:
			{
				typeString = "Mat3";
				auto m = std::get<glm::mat3>(uniformValue.value);
				valueString = GLMMatrixToString(m);

				break;
			}

			case UniformType_mat4:
			{
				typeString = "Mat4";
				auto m = std::get<glm::mat4>(uniformValue.value);
				valueString = GLMMatrixToString(m);
				break;
			}

			case UniformType_Sampler2D:
			case UniformType_Sampler3D:
			case UniformType_SamplerCube:
			{
				typeString = (uniformValue.type == UniformType_Sampler2D ? "Sampler2D" : uniformValue.type == UniformType_Sampler3D ? "Sampler3D" : "SamplerCube");

				//UUID texID = std::get<UUID>(uniformValue.value);
				//valueString = texID.Get();
				break;
			}
			default:
				break;

				
			}
			JsonNode propertyNode = propertiesNode.CreateObjectField(id);
			propertyNode.CreateField("type", typeString);
			propertyNode.CreateField("value", valueString);
		}

		jsonData.ToFile(filepath);
		metadata.IsOutdated = true;
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