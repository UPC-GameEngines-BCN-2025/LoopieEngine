#include "Mesh.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Resources/AssetRegistry.h"
#include "Loopie/Importers/MeshImporter.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace Loopie {
	Mesh::Mesh(const UUID& id, unsigned int index) : Resource(id)
	{
		m_meshIndex = index;
		Reload();
	}
	void Mesh::LoadFromFile(const std::string path)
	{
		MeshImporter::LoadModel(path, *this);
	}

	void Mesh::Reload()
	{
		Metadata* metadata = AssetRegistry::GetMetadata(GetUUID());
		if (metadata->HasCache && m_meshIndex< metadata->cachesPath.size()) {
			LoadFromFile(metadata->cachesPath[m_meshIndex]);
		}
	}
}
