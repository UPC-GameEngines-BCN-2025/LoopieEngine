#include "Mesh.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Resources/AssetRegistry.h"
#include "Loopie/Importers/MeshImporter.h"

#include <fstream>
#include <iostream>
#include <filesystem>

namespace Loopie {
	Mesh::Mesh(const UUID& id, unsigned int index) : Resource(id,ResourceType::MESH)
	{
		m_meshIndex = index;
		Load();
	}

	bool Mesh::Load()
	{
		Metadata* metadata = AssetRegistry::GetMetadata(GetUUID());
		if (metadata->HasCache && m_meshIndex< metadata->CachesPath.size()) {
			MeshImporter::LoadModel(metadata->CachesPath[m_meshIndex], *this);
			return true;
		}
		return false;
	}
}
