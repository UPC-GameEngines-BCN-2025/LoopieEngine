#include "Texture.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Importers/TextureImporter.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace Loopie {

	std::shared_ptr<Texture> Texture::s_Texture = nullptr;

	Texture::Texture(const UUID& id) : Resource(id, ResourceType::TEXTURE) {
		Load();
	}

	bool Texture::Load()
	{
		Metadata* metadata = AssetRegistry::GetMetadata(GetUUID());
		if (metadata->HasCache) {
			TextureImporter::LoadImage(metadata->CachesPath[0], *this);
			return true;
		}
		return false;
	}

	std::shared_ptr<Texture> Texture::GetDefault() {
		if (s_Texture)
			return s_Texture;
		Metadata& metadata = AssetRegistry::GetOrCreateMetadata("assets/textures/simpleWhiteTexture.png");
		if (!metadata.HasCache) {
			TextureImporter::ImportImage("assets/textures/simpleWhiteTexture.png", metadata);
		}
		s_Texture = ResourceManager::GetTexture(metadata);
		s_Texture->Load();
		return s_Texture;
	}
}