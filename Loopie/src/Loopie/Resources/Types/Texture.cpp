#include "Texture.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Resources/AssetRegistry.h"
#include "Loopie/Importers/TextureImporter.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace Loopie {
	Texture::Texture(const UUID& id) : Resource(id) {
		Reload();
	}

	void Texture::LoadFromFile(const std::string path)
	{		
		m_path = path;
		TextureImporter::LoadImage(path, *this);
	}

	void Texture::Reload()
	{
		Metadata* metadata = AssetRegistry::GetMetadata(GetUUID());
		if (metadata->HasCache) {
			LoadFromFile(metadata->cachesPath[0]);
		}
	}
}