#pragma once

#include "Loopie/Core/UUID.h"
#include "Loopie/Core/IIdentificable.h"

namespace Loopie {
	enum ResourceType
	{
		TEXTURE,
		MESH,
		MATERIAL,
		SHADER,
		SCENE,
		UNKNOWN
	};

	class Resource : public  IIdentificable {
	public:
		Resource(UUID uuid, ResourceType type) : m_uuid(uuid), m_type(type) {}
		virtual ~Resource();

		const UUID& GetUUID() { return m_uuid; }

		virtual bool Load() = 0;
	
		void IncrementReferenceCount() { m_referenceCount++; }
		void DecrementReferenceCount() { m_referenceCount--; }

	protected:
		UUID m_uuid;
		ResourceType m_type;
		unsigned int m_referenceCount = 0;

	};
}