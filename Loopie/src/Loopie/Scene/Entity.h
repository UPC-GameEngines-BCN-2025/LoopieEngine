#pragma once

#include "Loopie/Core/UUID.h"
#include "Loopie/Core/IIdentificable.h"

#include <string>
#include <vector>
#include <memory>

namespace Loopie {
	class Component;
	class Transform;
	class Scene;


	/// Maybe Add a CopyComponent
	class Entity : public std::enable_shared_from_this<Entity>
	{
	public:
		Entity(const std::string& name);
		~Entity();

		template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		T* AddComponent(Args&&... args)
		{
			if constexpr (std::is_same_v<T, Transform>) {
				if (m_transform)
					return GetTransform();
			}

			m_components.push_back(std::make_unique<T>(std::forward<Args>(args)...));
			T* componentPtr = static_cast<T*>(m_components.back().get());

			componentPtr->m_owner = weak_from_this();
			componentPtr->Init();

			if constexpr (std::is_same_v<T, Transform>) {
				m_transform = componentPtr;
			}

			return componentPtr;
		}


		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		T* GetComponent() const
		{
			for (const auto& component : m_components) {
				if (component->GetTypeID() == T::GetTypeIDStatic())
					return static_cast<T*>(component.get());
			}
			
			return nullptr;
		}

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		bool HasComponent() const
		{
			return GetComponent<T>() != nullptr;
		}

		// Removes first component of that specific type
		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
		bool RemoveComponent()
		{
			if constexpr (std::is_same_v<T, Transform>)
				return false;

			for (size_t i = 0; i < m_components.size(); i++)
			{
				if (m_components[i]->GetTypeID() == T::GetTypeIDStatic()){
					m_components.erase(m_components.begin() + i);
					return true;
				}
			}
			return false;
		}

		bool RemoveComponent(Component* component);

		// If a child is set up, then it means this is its parent and will update it accordingly
		void AddChild(const std::shared_ptr<Entity>& child);
		void RemoveChild(const std::shared_ptr<Entity>& child);
		void RemoveChild(UUID childUuid);

		const UUID& GetUUID() const;
		const std::string& GetName() const;
		bool GetIsActive() const;
		std::shared_ptr<Entity> GetChild(UUID uuid) const;
		const std::vector<std::shared_ptr<Entity>>& GetChildren() const;
		std::weak_ptr<Entity> GetParent() const;
		std::vector<Component*> GetComponents() const;
		Transform* GetTransform() const;

		void SetUUID(const std::string uuid);
		void SetUUID(UUID uuid);
		void SetName(const std::string& name);
		void SetIsActive(bool active);
		// If a parent is set up, then it means this is its child and will update it accordingly
		void SetParent(const std::shared_ptr<Entity>& parent);

	private:
		void GetRecursiveChildren(std::vector<std::shared_ptr<Entity>>& childrenEntities);

	private:
		std::weak_ptr<Entity> m_parentEntity;
		std::vector<std::shared_ptr<Entity>> m_childrenEntities;
		std::vector<std::unique_ptr<Component>> m_components; // Might want to re-do this to a map for optimization
		Transform* m_transform = nullptr;

		UUID m_uuid;
		std::string m_name;
		bool m_isActive = true;
	};
}