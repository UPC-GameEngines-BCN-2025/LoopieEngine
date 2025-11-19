#pragma once
#include "Loopie/Resources/Resource.h"
#include "Loopie/Resources/Types/Texture.h"
#include "Loopie/Resources/Types/Mesh.h"
#include "Loopie/Resources/Resource.h"
#include "Loopie/Resources/AssetRegistry.h"

#include <unordered_map>
#include <memory>
#include <cstddef>

namespace Loopie {

    struct ResourceKey {
        Metadata metadata;
        int index = 0;

        bool operator==(const ResourceKey& other) const noexcept {
            return metadata.uuid == other.metadata.uuid && index == other.index;
        }
    };

    struct ResourceKeyHash {
        std::size_t operator()(const ResourceKey& key) const noexcept {
            return std::hash<UUID>()(key.metadata.uuid) ^ (std::hash<int>()(key.index) << 1);
        }
    };

    class ResourceManager {    
    public:

        static std::shared_ptr<Texture> GetTexture(const Metadata& metadata);
        static std::shared_ptr<Mesh> GetMesh(const Metadata& metadata, int index);

    private:
        static std::shared_ptr<Resource> GetResource(const ResourceKey& key);

    private:
		static std::unordered_map<ResourceKey, std::shared_ptr<Resource>, ResourceKeyHash> m_Resources;
    };
}