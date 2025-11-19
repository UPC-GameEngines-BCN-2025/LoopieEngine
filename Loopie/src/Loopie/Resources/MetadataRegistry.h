#pragma once

#include "Loopie/Core/UUID.h"

#include <filesystem>
#include <string>
#include <chrono>

#include <iomanip>
#include <sstream>
#include <ctime>
#include <vector>   

namespace Loopie {

    struct Metadata {
        UUID uuid;
        std::vector<std::string> cachesPath;

        bool HasCache = false;
        bool IsOutdated = false;
        std::time_t LastModified = 0;
        std::string LegibleLastModified;

        const std::string& RefreshLegibleLastModified() {
            std::tm* tm_ptr = std::localtime(&LastModified);
            std::ostringstream oss;
            oss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
            LegibleLastModified = oss.str();

            return LegibleLastModified;
        }
    };

    class MetadataRegistry {
    public:
        static Metadata GetMetadataAsset(const std::string& assetPath);
        static void SaveMetadata(const std::filesystem::path& assetPath, const Metadata& metadata);

        static bool IsMetadataFile(const std::filesystem::path& assetPath);
        static std::time_t GetLastModifiedFromPath(const std::string& assetPath);
    };
}