#include "Scene.h"
#include "Loopie/Files/Json.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Components/Camera.h"
#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Helpers/LoopieHelpers.h"

#include <unordered_set>


namespace Loopie {
	Scene::Scene(const std::string& filePath)
	{
		m_filePath = filePath;

		m_rootEntity = std::make_shared<Entity>("scene");
		m_rootEntity->AddComponent<Transform>();

		//ReadAndLoadSceneFile(std::string("TESTSavedScene.json"));
		//ReadAndLoadSceneFile(filePath);
	}

	Scene::~Scene()
	{
		m_entities.clear();
	}

	void Scene::SaveScene(const std::string* filePath)
	{
		JsonData saveData;
		JsonNode entitiesObj = saveData.CreateArrayField("entities");

		// Pol comment to understand json::objects and json::arrays
		// Array field = values
		// Object field = kvm
		
		for (const auto& [id, entity] : GetAllEntities())
		{
			//json entityObj = json::object();
			JsonData entityObj = JsonData();
			
			entityObj.CreateField<std::string>("uuid", id.Get());
			entityObj.CreateField<std::string>("name", entity->GetName());
			entityObj.CreateField<bool>("active", entity->GetIsActive());


			if (std::shared_ptr<Entity> parentEntity = entity->GetParent().lock())
				entityObj.CreateField<std::string>("parent_uuid", parentEntity->GetUUID().Get());

			// Creates an array of components
			JsonNode componentsObj = entityObj.CreateArrayField("components");
			
			for (auto const& component : entity->GetComponents())
			{
				JsonData componentObj = JsonData();
				component->Serialize(componentObj.Node());
				componentsObj.AddArrayElement(componentObj.GetRoot());
			}

			saveData.AddArrayElement("entities", entityObj.GetRoot());
		}

		saveData.ToFile("TESTSavedScene.json");
		Log::Info("Scene saved.");
	}

	std::shared_ptr<Entity> Scene::CreateEntity(const std::string& name,
												std::shared_ptr<Entity> parentEntity)
	{
		std::string uniqueName = GetUniqueName(parentEntity, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		if (!parentEntity)
			m_rootEntity->AddChild(entity);
		else
			parentEntity->AddChild(entity);

		entity->AddComponent<Transform>();

		m_entities[entity->GetUUID()] = entity;
		return entity;
	}

	std::shared_ptr<Entity> Scene::CreateEntity(const UUID& uuid, const std::string& name,
												 std::shared_ptr<Entity> parentEntity)
	{
		std::string uniqueName = GetUniqueName(parentEntity, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		entity->SetUUID(uuid);

		if (!parentEntity)
			m_rootEntity->AddChild(entity);
		else
			parentEntity->AddChild(entity);

		entity->AddComponent<Transform>();

		m_entities[entity->GetUUID()] = entity;
		return entity;
	}

	std::shared_ptr<Entity> Scene::CreateEntity(const vec3& position, const quaternion& rotation, const vec3& scale,
												std::shared_ptr<Entity> parentEntity, const std::string& name)
	{
		std::string uniqueName = GetUniqueName(parentEntity, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		if (!parentEntity)
			m_rootEntity->AddChild(entity);
		else
			parentEntity->AddChild(entity);

		entity->AddComponent<Transform>(position, rotation, scale);
		m_entities[entity->GetUUID()] = entity;

		return entity;
	}

	std::shared_ptr<Entity> Scene::CreateEntity(Transform* transform,
										std::shared_ptr<Entity> parentEntity,
										const std::string& name)
	{
		std::string uniqueName = GetUniqueName(parentEntity, name);
		std::shared_ptr<Entity> entity = std::make_shared<Entity>(uniqueName);
		if (!parentEntity)
			m_rootEntity->AddChild(entity);
		else
			parentEntity->AddChild(entity);

		if (!transform)
		{
			entity->AddComponent<Transform>();
		}
		else
		{
			entity->AddComponent<Transform>(*transform);
		}
		m_entities[entity->GetUUID()] = entity;

		return entity;
	}

	void Scene::RemoveEntity(UUID uuid)
	{
		auto it = m_entities.find(uuid);
		if (it == m_entities.end())
			return;

		RemoveEntityRecursive(it->second);
	}

	void Scene::RemoveEntity(std::shared_ptr<Entity> entity)
	{
		if (!entity) 
			return;

		RemoveEntityRecursive(entity);
	}

	std::shared_ptr<Entity> Scene::GetRootEntity() const
	{
		return m_rootEntity;
	}

	std::shared_ptr<Entity> Scene::GetEntity(UUID uuid) const
	{
		auto it = m_entities.find(uuid);
		return (it != m_entities.end()) ? it->second : nullptr;
	}

	std::shared_ptr<Entity> Scene::GetEntity(const std::string& name) const
	{
		for (const auto& [uuid, entity] : m_entities)
		{
			if (entity->GetName() == name)
			{
				return entity;
			}
		}
		return nullptr;
	}

	const std::unordered_map<UUID, std::shared_ptr<Entity>>& Scene::GetAllEntities() const
	{
		return m_entities;
	}

	std::vector<std::shared_ptr<Entity>> Scene::GetAllEntitiesHierarchical(std::shared_ptr<Entity> parentEntity) const
	{
		std::vector<std::shared_ptr<Entity>> entities;

		if (!parentEntity)
		{
			parentEntity = m_rootEntity;
		}

		CollectEntitiesRecursive(parentEntity, entities);
		return entities;
	}

	std::vector<std::shared_ptr<Entity>> Scene::GetAllSiblings(std::shared_ptr<Entity> parentEntity) const
	{
		std::vector<std::shared_ptr<Entity>> siblingEntities;

		if (!parentEntity)
		{
			parentEntity = m_rootEntity;
		}

		for (const auto& child : parentEntity->GetChildren())
		{
			siblingEntities.push_back(child);
		}

		return siblingEntities;
	}

	void Scene::ReadAndLoadSceneFile(std::string filePath)
	{
		// TODO: This should read the serialized file and load the entities and their uuid
		// We can use the hierarchy for this

		m_entities.clear();

		// TEMP
		m_rootEntity = std::make_shared<Entity>("scene");
		m_rootEntity->AddComponent<Transform>();
		// END_TEMP
		
		//JsonData saveData = Json::ReadFromFile(*filePath);
		JsonData saveData = Json::ReadFromFile("TESTSavedScene.json");

		if (saveData.IsEmpty())
		{
			Log::Error("Failed to load scene file");
			return;
		}

		JsonNode rootNode = saveData.Child("entities");

		if (!rootNode.IsValid() || !rootNode.IsArray())
		{
			Log::Error("No entities array in scene file.");
			return;
		}

		// Map used to store created entities by UUID for later linking

		// First iteration: Create all entities
		for (unsigned int i = 0; i < rootNode.Size(); ++i)
		{
			JsonResult<json> entityJson = rootNode.GetArrayElement<json>(i);
			JsonNode entityNode = JsonNode(&entityJson.Result);

			if (!entityNode.IsValid() || !entityNode.Contains("uuid") || !entityNode.Contains("name"))
				continue;

			UUID uuid = UUID(entityNode.GetValue<std::string>("uuid").Result);
			std::string name = entityNode.GetValue<std::string>("name").Result;
			bool active = entityNode.GetValue<bool>("active", false).Result;

			std::shared_ptr<Entity> entity = CreateEntity(uuid, name);
			entity->SetIsActive(active);		
		}

		// Second iteration: Link relationships and components
		for (size_t i = 0; i < rootNode.Size(); ++i)
		{
			JsonResult<json> entityJson = rootNode.GetArrayElement<json>(i);
			JsonNode entityNode = JsonNode(&entityJson.Result);

			if (!entityNode.IsValid() || !entityNode.Contains("uuid"))
				continue;

			UUID uuid = UUID(entityNode.GetValue<std::string>("uuid").Result);
			std::shared_ptr<Entity> entity = m_entities[uuid];

			// Set parent if it exists
			if (entityNode.Contains("parent_uuid"))
			{
				UUID parentUUID = UUID(entityNode.GetValue<std::string>("parent_uuid").Result);
				if (m_entities.find(parentUUID) != m_entities.end())
				{
					entity->SetParent(m_entities[parentUUID]);
				}
			}

			JsonNode componentsObj = entityNode.Child("components");
			if (componentsObj.IsValid() && componentsObj.IsArray())
			{

				for (size_t j = 0; j < componentsObj.Size(); ++j)
				{
					JsonResult<json> componentJson = componentsObj.GetArrayElement<json>(j);
					JsonNode componentNode = JsonNode(&componentJson.Result);

					// Check which component type it is - I don't know if we can do this more clean?
					
					if (componentNode.Contains("transform"))
					{
						JsonNode node = componentNode.Child("transform");
						entity->GetTransform()->Deserialize(node);
					}
					else if (componentNode.Contains("camera"))
					{
						JsonNode node = componentNode.Child("camera");
						auto camera = entity->AddComponent<Camera>();
						if (camera)
						{
							camera->Deserialize(node);
						}
					}
					else if (componentNode.Contains("meshrenderer"))
					{
						JsonNode node = componentNode.Child("meshrenderer");
						auto meshRenderer = entity->AddComponent<MeshRenderer>();
						if (meshRenderer)
						{
							meshRenderer->Deserialize(node);
						}
					}
				}
			}
		}
		Log::Info("Scene loaded successfully");
	}

	std::string Scene::GetUniqueName(std::shared_ptr<Entity> parentEntity, const std::string& desiredName)
	{
		if (!parentEntity)
			return desiredName;

		std::vector<std::string> existingNames;
		for (const auto& sibling : GetAllSiblings(parentEntity))
		{
			existingNames.emplace_back(sibling->GetName());
		}

		return Helper::MakeUniqueName(desiredName, existingNames);
	}

	void Scene::CollectEntitiesRecursive(std::shared_ptr<Entity> entity,
		std::vector<std::shared_ptr<Entity>>& outEntities) const
	{
		if (!entity)
			return;

		outEntities.push_back(entity);

		for (const auto& child : entity->GetChildren())
		{
			CollectEntitiesRecursive(child, outEntities);
		}
	}

	void Scene::RemoveEntityRecursive(std::shared_ptr<Entity> entity)
	{
		if (!entity) 
			return;

		auto children = entity->GetChildren(); // Making a copy instead of referencing just in case

		for (const auto& child : children)
		{
			RemoveEntityRecursive(child);
		}

		if (std::shared_ptr<Entity> parent = entity->GetParent().lock())
		{
			parent->RemoveChild(entity->GetUUID());
		}

		m_entities.erase(entity->GetUUID());
	}
}
