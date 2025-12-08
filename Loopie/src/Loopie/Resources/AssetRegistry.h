#pragma once

#include "Loopie/Core/UUID.h"
#include "Loopie/Resources/MetadataRegistry.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace Loopie {

    class AssetRegistry {
    public:
        static void Initialize();
        static void Shutdown();

        static void RefreshAssetRegistry();

        static Metadata& GetOrCreateMetadata(const std::filesystem::path& assetPath);

        static Metadata* GetMetadata(const UUID& uuid);
        static Metadata* GetMetadata(const std::string& sourcePath);
        static const std::string GetSourcePath(const UUID& uuid);

        static bool UpdateMetadata(const Metadata& metadata, const std::filesystem::path& assetPath);

        static void Clear();

    private:
        static void ScanEngineDirectory();
        static void ScanAssetDirectory();
        static void CleanOrphanedMetadata();
        static void CleanOrphanedLibraryFiles();
        static void Register(const std::string& path,const Metadata& metadata);

    private:
        static std::unordered_map<UUID, Metadata> s_Assets;
        static std::unordered_map<std::string, UUID> s_PathToUUID;
        static std::unordered_map<UUID, std::string> s_UUIDToPath;
    };
}