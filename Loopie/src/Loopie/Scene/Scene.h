#pragma once

#include "Loopie/Core/UUID.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Math/MathTypes.h"

#include <string>
#include <unordered_map>
	
namespace Loopie {
	class Scene
	{
	public:
		Scene(const std::string& filePath);
		~Scene();

		void SaveScene(const std::string* filePath = nullptr);

		std::shared_ptr<Entity> CreateEntity(const std::string& name = "Entity",
											 std::shared_ptr<Entity> parentEntity = nullptr);
		std::shared_ptr<Entity> CreateEntity(const UUID& uuid, 
											const std::string& name = "Entity",
											std::shared_ptr<Entity> parentEntity = nullptr);
		std::shared_ptr<Entity> CreateEntity(const vec3& position = { 0, 0, 0 }, 
											const quaternion& rotation = { 1, 0, 0, 0 }, 
											const vec3& scale = { 1, 1, 1 },
											std::shared_ptr<Entity> parentEntity = nullptr,
											const std::string& name = "Entity");
		std::shared_ptr<Entity> CreateEntity(Transform* transform = nullptr,
											std::shared_ptr<Entity> parentEntity = nullptr,
											const std::string& name = "Entity");
		
		void RemoveEntity(UUID uuid);
		void RemoveEntity(std::shared_ptr<Entity> entity);
		// Name should be trimmed out of spaces for changename to work
		
		std::shared_ptr<Entity> GetRootEntity() const;
		std::shared_ptr<Entity> GetEntity(UUID uuid) const;
		std::shared_ptr<Entity> GetEntity(const std::string& name) const;
		// Returns the unordered_map of the UUID and Entity ptrs
		// Usage: for (const auto& [uuid, entity] : scene.GetAllEntities()) {entity->Update();}
		const std::unordered_map<UUID, std::shared_ptr<Entity>>& GetAllEntities() const;
		std::vector<std::shared_ptr<Entity>> GetAllEntitiesHierarchical(std::shared_ptr<Entity> parentEntity = nullptr) const;
		std::vector<std::shared_ptr<Entity>> GetAllSiblings(std::shared_ptr<Entity> parentEntity = nullptr) const;
		void ReadAndLoadSceneFile(std::string filePath);

	private:
		std::string GetUniqueName(std::shared_ptr<Entity> parentEntity, const std::string& desiredName);
		void CollectEntitiesRecursive(std::shared_ptr<Entity> entity,
									  std::vector<std::shared_ptr<Entity>>& outEntities) const;
		void RemoveEntityRecursive(std::shared_ptr<Entity> parent);

	private:
		std::unordered_map<UUID, std::shared_ptr<Entity>> m_entities; // Fast lookup
		std::shared_ptr<Entity> m_rootEntity; // Hierarchy based
		std::string m_filePath;
	};	
}