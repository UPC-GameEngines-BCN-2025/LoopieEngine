#include "ResourceManager.h"



namespace Loopie {
	std::unordered_map<ResourceKey, std::shared_ptr<Resource>, ResourceKeyHash> ResourceManager::m_Resources;

    std::shared_ptr<Texture> ResourceManager::GetTexture(const Metadata& metadata) {
        ResourceKey key{ metadata, 0 };
        auto resource = GetResource(key);
        if (resource) {
            return std::static_pointer_cast<Texture>(resource);
        }
        auto texture = std::make_shared<Texture>(metadata.UUID);
        m_Resources[key] = texture;
        return texture;
    }

    std::shared_ptr<Mesh> ResourceManager::GetMesh(const Metadata& metadata, int index) {
        ResourceKey key{ metadata, index };
        auto resource = GetResource(key);
        if (resource) {
            return std::static_pointer_cast<Mesh>(resource);
        }
        auto mesh = std::make_shared<Mesh>(metadata.UUID, index);
        m_Resources[key] = mesh;
        return mesh;
    }

    std::shared_ptr<Material> ResourceManager::GetMaterial(const Metadata& metadata)
    {
        ResourceKey key{ metadata, 0 };
        auto resource = GetResource(key);
        if (resource) {
            return std::static_pointer_cast<Material>(resource);
        }
        auto material = std::make_shared<Material>(metadata.UUID);
        m_Resources[key] = material;
        return material;
    }

    std::shared_ptr<Resource> ResourceManager::GetResource(const ResourceKey& key) {
        auto it = m_Resources.find(key);
        if (it != m_Resources.end()) {
            return it->second;
        }
        return nullptr;
    }
}
