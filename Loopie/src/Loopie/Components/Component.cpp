#include "Component.h"


#include "Loopie/Scene/Entity.h"
#include "Loopie/Components/Transform.h"

namespace Loopie {
	Component::~Component(){}
	Transform* Component::GetTransform() const
	{
		return GetOwner()->GetTransform();
	}

	const UUID& Component::GetUUID() const
	{
		return m_uuid;
	}

	bool Component::GetIsActive() const
	{
		return m_isActive && m_owner.lock()->GetIsActive();
	}

	void Component::SetIsActive(bool active)
	{
		m_isActive = active;
	}

	void Component::SetUUID(const std::string uuid)
	{
		m_uuid = UUID(uuid);
	}

	JsonNode Component::Serialize(JsonNode& parent) const // TEMP DELETE THIS
	{
		return JsonNode();
	}

	void Component::Deserialize(const JsonNode& data)// TEMP DELETE THIS
	{
		return;
	}
}