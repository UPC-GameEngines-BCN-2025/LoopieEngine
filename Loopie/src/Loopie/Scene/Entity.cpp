#include "Entity.h"

#include "Loopie/Components/Transform.h"
#include "Loopie/Core/Log.h"

#include <algorithm>

namespace Loopie {
	Entity::Entity(const std::string& name) : m_name(name)
	{
		
	}

	Entity::~Entity()
	{
		m_components.clear();
		m_childrenEntities.clear();
	}

	bool Entity::RemoveComponent(Component* component)
	{
		if (component->GetTypeID() == m_transform->GetTypeID())
			return false;

		for (size_t i = 0; i < m_components.size(); i++)
		{
			if (m_components[i].get() == component) {
				m_components.erase(m_components.begin() + i);
				return true;
			}
		}
		return false;
	}

	void Entity::AddChild(const std::shared_ptr<Entity>& child)
	{
		if (child && child.get() != this)
		{
			std::shared_ptr<Entity> childParent = child->m_parentEntity.lock();
			if (childParent)
			{
				childParent->RemoveChild(child);
			}

			m_childrenEntities.push_back(child);
			child->m_parentEntity = weak_from_this();
		}
	}

	void Entity::RemoveChild(const std::shared_ptr<Entity>& child)
	{
		auto it = std::find(m_childrenEntities.begin(), m_childrenEntities.end(), child);
		if (it != m_childrenEntities.end())
		{
			(*it)->m_parentEntity.reset();
			m_childrenEntities.erase(it);
		}
	}

	void Entity::RemoveChild(UUID childUuid)
	{
		for (auto it = m_childrenEntities.begin(); it != m_childrenEntities.end(); ++it)
		{
			if ((*it)->GetUUID() == childUuid)
			{
				(*it)->m_parentEntity.reset();
				m_childrenEntities.erase(it);
				return;
			}
		}
	}

	const UUID& Entity::GetUUID() const
	{
		return m_uuid;
	}

	const std::string& Entity::GetName() const
	{
		return m_name;
	}

	bool Entity::GetIsActive() const
	{
		return m_isActive;
	}

	std::shared_ptr<Entity> Entity::GetChild(UUID uuid) const
	{
		for (const auto& child : m_childrenEntities)
		{
			if (child->GetUUID() == uuid)
			{
				return child;
			}
		}
		return nullptr;
	}

	const std::vector<std::shared_ptr<Entity>>& Entity::GetChildren() const
	{
		return m_childrenEntities;
	}

	std::weak_ptr<Entity> Entity::GetParent() const
	{ 
		return m_parentEntity;
	}

	std::vector<Component*> Entity::GetComponents() const
	{
		std::vector<Component*> outComponents;
		outComponents.reserve(m_components.size());

		for (const auto& component : m_components) {
			outComponents.push_back(component.get());
		}

		return outComponents;
	}

	Transform* Entity::GetTransform() const
	{
		return m_transform;
	}

	void Entity::SetUUID(const std::string uuid)
	{
		m_uuid = UUID(uuid);
	}

	void Entity::SetUUID(UUID uuid)
	{
		m_uuid = uuid;
	}

	void Entity::SetName(const std::string& name)
	{
		m_name = name;
	}

	void Entity::SetIsActive(bool active)
	{
		m_isActive = active;
	}

	void Entity::SetParent(const std::shared_ptr<Entity>& parent)
	{
		// Prevents parenting to its own son
		if (parent == shared_from_this())
		{
			Log::Warn("Cannot parent entity to itself.");
			return;
		}

		if (parent)
		{
			std::vector<std::shared_ptr<Entity>> allChildren;
			GetRecursiveChildren(allChildren);

			for (const auto& child : allChildren)
			{
				if (parent == child) 
				{
					Log::Warn("Cannot parent entity to one of its descendants, as it would create an infinite loop.");
					return;
				}
			}
		}


		std::shared_ptr<Entity> currentParent = m_parentEntity.lock();
		if (currentParent)
		{
			currentParent->RemoveChild(shared_from_this());
		}

		m_parentEntity = parent;

		if (parent && (parent != shared_from_this()))
		{
			parent->AddChild(shared_from_this());
		}
	}

	void Entity::GetRecursiveChildren(std::vector<std::shared_ptr<Entity>>& childrenEntities) 
	{
		for (const auto& child : m_childrenEntities)
		{
			childrenEntities.push_back(child);
			child->GetRecursiveChildren(childrenEntities);
		}
	}
}