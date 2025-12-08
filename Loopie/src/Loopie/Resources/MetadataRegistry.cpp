#include "MetadataRegistry.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Files/Json.h"
#include "Loopie/Files/DirectoryManager.h"

namespace Loopie {
	Metadata MetadataRegistry::GetMetadataAsset(const std::string& assetPath)
	{
		std::filesystem::path metadataPath = assetPath + ".meta";

        if (!std::filesystem::exists(metadataPath)) {
            Metadata metadata;
            metadata.IsOutdated = true;
            metadata.LastModified = GetLastModifiedFromPath(assetPath);
            SaveMetadata(assetPath, metadata);
            return metadata;
        }
        else {
            Metadata metadata;

            JsonData data = Json::ReadFromFile(metadataPath);
            metadata.UUID = UUID(data.GetValue<std::string>("Id").Result);
            metadata.Type = (ResourceType)data.GetValue<int>("Type").Result;
            metadata.HasCache = data.GetValue<bool>("HasCache").Result;
            metadata.LastModified = data.GetValue<std::time_t>("LastModified").Result;
            std::time_t currentTime = GetLastModifiedFromPath(assetPath);
            metadata.IsOutdated = currentTime != metadata.LastModified;

            if (metadata.HasCache) {
                JsonNode cacheNode = data.Child("Caches");
                unsigned int entries = cacheNode.Size();
                Project project = Application::GetInstance().m_activeProject;
                for (unsigned int i = 0; i < entries; i++)
                {
                    std::string cachePath = cacheNode.GetArrayElement<std::string>(i).Result;
                    if (!std::filesystem::exists(project.GetChachePath() / cachePath))
                    {
                        metadata.cachesPath.clear();
                        metadata.HasCache = false;
                        break;
                    }
                    metadata.cachesPath.push_back(cachePath);
                }
            }
            metadata.RefreshLegibleLastModified();
            return metadata;
        }
	}

	void MetadataRegistry::SaveMetadata(const std::filesystem::path& assetPath, const Metadata& metadata)
	{
        std::filesystem::path metadataPath = assetPath.string() + ".meta";

        JsonData data;
        data.CreateField<std::string>("Id",metadata.UUID.Get());
        data.CreateField<int>("Type",metadata.Type);
        data.CreateField<bool>("HasCache", metadata.HasCache);
        data.CreateField<time_t>("LastModified", metadata.LastModified);

        if (metadata.HasCache) {
            data.CreateArrayField("Caches");
            for (const auto& paths : metadata.cachesPath)
            {
                data.AddArrayElement<std::string>("Caches", paths);
            }
        }
       
        data.ToFile(metadataPath);
	}
    bool MetadataRegistry::IsMetadataFile(const std::filesystem::path& assetPath)
    {
        return assetPath.extension() == ".meta";
    }

    std::time_t MetadataRegistry::GetLastModifiedFromPath(const std::string& assetPath) {

        auto ftime = std::filesystem::last_write_time(assetPath);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now()
        );
        return std::chrono::system_clock::to_time_t(sctp);
    }
}