#include "HierarchyInterface.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Importers/MeshImporter.h"

#include <imgui.h>

namespace Loopie {
	std::shared_ptr<Entity> HierarchyInterface::s_SelectedEntity = nullptr;

	HierarchyInterface::HierarchyInterface() {
		
	}

	void HierarchyInterface::Update(const InputEventManager& inputEvent)
	{
		if(m_focused)
			HotKeysSelectedEntiy(inputEvent);
	}

	void HierarchyInterface::Render() {

		if (ImGui::Begin("Hierarchy")) {

			m_focused = ImGui::IsWindowHovered();

			if (!m_scene) {
				ImGui::End();
				return;
			}

			if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)))
				SelectEntity(nullptr);


			for (const auto& entity : m_scene->GetRootEntity()->GetChildren())
			{
				DrawEntitySlot(entity);
				
			}
			
			if (ImGui::BeginPopupContextWindow("HierarchyBackgroundContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
				DrawContextMenu(nullptr);
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

	void HierarchyInterface::SetScene(Scene* scene)
	{
		m_scene = scene;
	}

	void HierarchyInterface::SelectEntity(std::shared_ptr<Entity> entity)
	{
		s_SelectedEntity = entity;
	}

	void HierarchyInterface::DrawEntitySlot(const std::shared_ptr<Entity>& entity)
	{
		if (!entity) {
			return;
		}
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		const auto& children = entity->GetChildren();
		bool hasChildren = !children.empty();

		if (!hasChildren)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		if (s_SelectedEntity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool opened = ImGui::TreeNodeEx((void*)entity.get(), flags, entity->GetName().c_str());

		Drag(entity);
		Drop(entity);

		if (ImGui::IsItemClicked())
		{
			////Expand To Select Multiple
			SelectEntity(entity);
		}

		if (ImGui::BeginPopupContextItem())
		{
			DrawContextMenu(entity);
			ImGui::EndPopup();
		}

		if (opened && hasChildren)
		{
			for (const auto& child : children)
			{
				DrawEntitySlot(child);
			}
			ImGui::TreePop();
		}
	}

	void HierarchyInterface::DrawContextMenu(const std::shared_ptr<Entity>& entity)
	{

		//// EXPAND MAYBE WITH A CUSTOM CREATOR -> MenuItem class (contains an Execute function, label, active Condition)???

		if (ImGui::MenuItem("Create Empty"))
		{
			std::shared_ptr<Entity> newEntity = m_scene->CreateEntity("Entity", entity);
			SelectEntity(newEntity);
		}	

		/*if (ImGui::MenuItem("Copy"))
		{

		}

		if (ImGui::MenuItem("Cut"))
		{

		}

		if (ImGui::MenuItem("Paste"))
		{

		}*/

		if (ImGui::MenuItem("Delete",nullptr, false, entity != nullptr))
		{
			if (s_SelectedEntity == entity)
				SelectEntity(nullptr);
			m_scene->RemoveEntity(entity->GetUUID());
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("3D Object"))
		{
			if (ImGui::MenuItem("Cube"))
				SelectEntity(CreatePrimitiveModel("assets/models/primitives/cube.fbx", "Cube", entity));

			if (ImGui::MenuItem("Sphere"))
				SelectEntity(CreatePrimitiveModel("assets/models/primitives/sphere.fbx", "Sphere", entity));

			if (ImGui::MenuItem("Cylinder"))
				SelectEntity(CreatePrimitiveModel("assets/models/primitives/cylinder.fbx", "Cylinder", entity));

			if (ImGui::MenuItem("Plane"))
				SelectEntity(CreatePrimitiveModel("assets/models/primitives/plane.fbx", "Plane", entity));

			ImGui::EndMenu();
		}
	}

	void HierarchyInterface::HotKeysSelectedEntiy(const InputEventManager& inputEvent)
	{
		if (!s_SelectedEntity) {
			return;
		}

		if (inputEvent.GetKeyStatus(SDL_SCANCODE_DELETE) == KeyState::DOWN) {
			m_scene->RemoveEntity(s_SelectedEntity->GetUUID());
			SelectEntity(nullptr);
		}

		if (inputEvent.GetKeyWithModifier(SDL_SCANCODE_C, KeyModifier::CTRL)) {
			/// Copy
		}

		if (inputEvent.GetKeyWithModifier(SDL_SCANCODE_V, KeyModifier::CTRL)) {
			/// Paste
		}

		if (inputEvent.GetKeyWithModifier(SDL_SCANCODE_X, KeyModifier::CTRL)) {
			/// Cut
		}

	}
	void HierarchyInterface::Drag(const std::shared_ptr<Entity>& entity)
	{
		if (ImGui::BeginDragDropSource())
		{
			Entity* rawPtr = entity.get();
			ImGui::SetDragDropPayload("HIERARCHY_ENTITY_PTR", &rawPtr, sizeof(Entity*));
			ImGui::EndDragDropSource();
		}
	}

	void HierarchyInterface::Drop(const std::shared_ptr<Entity>& entity)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ENTITY_PTR"))
			{
				Entity* draggedRaw = *(Entity**)payload->Data;

				if(draggedRaw)
					draggedRaw->SetParent(entity);
			}

			ImGui::EndDragDropTarget();
		}
	}


	std::shared_ptr<Entity> HierarchyInterface::CreatePrimitiveModel(const std::string& modelPath, const std::string& name, const std::shared_ptr<Entity>& parent)
	{
		std::shared_ptr<Entity> newEntity = m_scene->CreateEntity(name, parent);
		MeshRenderer* renderer = newEntity->AddComponent<MeshRenderer>();

		Metadata& meta = AssetRegistry::GetOrCreateMetadata(modelPath);
		MeshImporter::ImportModel(modelPath, meta);
		std::shared_ptr<Mesh> mesh = ResourceManager::GetMesh(meta, 0);
		if (mesh)
			renderer->SetMesh(mesh);

		return newEntity;
	}
}