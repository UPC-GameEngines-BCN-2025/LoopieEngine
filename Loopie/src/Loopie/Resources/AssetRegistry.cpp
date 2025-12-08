#include "AssetRegistry.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Files/DirectoryManager.h"


#include "Loopie/Importers/TextureImporter.h"
#include "Loopie/Importers/MeshImporter.h"
#include "Loopie/Importers/MaterialImporter.h"

#include <filesystem>
#include <unordered_set>

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

				if (metadata.Type == ResourceType::TEXTURE || TextureImporter::CheckIfIsImage(pathString.c_str())) {
					TextureImporter::ImportImage(pathString, metadata);
				}
				else if (metadata.Type == ResourceType::MESH || MeshImporter::CheckIfIsModel(pathString.c_str())) {
					MeshImporter::ImportModel(pathString, metadata);
				}
				else if (metadata.Type == ResourceType::MATERIAL || MaterialImporter::CheckIfIsMaterial(pathString.c_str())) {
					MaterialImporter::ImportMaterial(pathString, metadata);
				}

				///
				UpdateMetadata(metadata, pathString);
			}
		}

		CleanOrphanedLibraryFiles();

		Application::GetInstance().m_notifier.Notify(EngineNotification::OnAssetRegistryReload);
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
					DirectoryManager::Delete(entry.path());
					Log::Warn("Removing orphaned meta file: {}", entry.path().string());
				}
			}
		}
	}

	void AssetRegistry::CleanOrphanedLibraryFiles()
	{
		const Project& project = Application::GetInstance().m_activeProject;
		const std::filesystem::path libraryPath = project.GetChachePath();

		if (!std::filesystem::exists(libraryPath))
			return;

		std::unordered_set<std::string> validCacheFiles;
		validCacheFiles.reserve(s_Assets.size() * 4);

		for (auto& [uuid, metadata] : s_Assets)
		{
			for (const std::string& cachePath : metadata.CachesPath)
			{
				if (cachePath.empty())
					continue;


				std::filesystem::path abs = libraryPath /cachePath;

				if (std::filesystem::exists(abs))
					validCacheFiles.insert(abs.string());
			}
		}

		for (auto& entry : std::filesystem::recursive_directory_iterator(libraryPath))
		{
			if (!entry.is_regular_file())
				continue;

			std::filesystem::path filePath = std::filesystem::absolute(entry.path());
			std::string fileStr = filePath.string();


			if (!validCacheFiles.count(fileStr))
			{
				Log::Warn("Removing orphaned library file: {}", fileStr);
				DirectoryManager::Delete(fileStr);
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

	const std::string AssetRegistry::GetSourcePath(const UUID& uuid)
	{
		auto it = s_UUIDToPath.find(uuid);
		if (it == s_UUIDToPath.end())
			return "";

		return it->second;
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