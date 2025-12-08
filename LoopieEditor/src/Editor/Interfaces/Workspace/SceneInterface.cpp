#include "SceneInterface.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Core/Application.h"
#include "Loopie/Render/Renderer.h"
#include "Loopie/Components/Transform.h"

#include "Loopie/Resources/AssetRegistry.h"
#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Importers/MeshImporter.h"
#include "Loopie/Importers/MaterialImporter.h"
#include "Loopie/Importers/TextureImporter.h"
#include "Loopie/Components/MeshRenderer.h"

#include "Editor/Interfaces/Workspace/HierarchyInterface.h"

#include <imgui.h>
#include <ImGuizmo.h>

namespace Loopie {
	SceneInterface::SceneInterface() {
		m_camera = std::make_shared<OrbitalCamera>();
		m_buffer = std::make_shared<FrameBuffer>(1,1);
		m_camera->GetCamera()->GetTransform()->SetPosition({ 0,5,-10.f });

		std::vector<std::string> iconsToLoad = {
			"assets/icons/icon_move.png",
			"assets/icons/icon_rotate.png",
			"assets/icons/icon_scale.png",
			"assets/icons/icon_trs.png"
		};

		std::vector<Metadata> iconsToLoadMetadatas;
		for (size_t i = 0; i < iconsToLoad.size(); i++)
		{
			Metadata& meta = AssetRegistry::GetOrCreateMetadata(iconsToLoad[i]);
			TextureImporter::ImportImage(iconsToLoad[i], meta);
			iconsToLoadMetadatas.emplace_back(meta);
		}

		m_moveIcon = ResourceManager::GetTexture(iconsToLoadMetadatas[0]);
		m_rotateIcon = ResourceManager::GetTexture(iconsToLoadMetadatas[1]);
		m_scaleIcon = ResourceManager::GetTexture(iconsToLoadMetadatas[2]);
		m_trsIcon = ResourceManager::GetTexture(iconsToLoadMetadatas[3]);


		m_gizmoOperation = ImGuizmo::TRANSLATE;
		m_gizmoMode = ImGuizmo::WORLD;
	}

	void SceneInterface::Update(const InputEventManager& inputEvent)
	{
		
		if (!m_focused && !m_interacted)
			return;
		
		m_camera->ProcessEvent(inputEvent);
		m_camera->Update();

	}

	void SceneInterface::Render() {
		const bool gizmoActive = ImGuizmo::IsOver() || ImGuizmo::IsUsing();
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav;
		if (gizmoActive)
		{
			flags |= ImGuiWindowFlags_NoMove;
		}

		if (ImGui::Begin("Scene",nullptr, flags)) {
			ImVec2 size = ImGui::GetContentRegionAvail();
			m_windowSize = { (int)size.x, (int)size.y };
			m_focused = ImGui::IsWindowHovered();
			ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
			ImVec2 cursorPos = ImGui::GetCursorPos();
			

			if (!m_focused && m_interacted || m_focused)
				m_interacted = ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right) || ImGui::IsMouseDown(ImGuiMouseButton_Middle);
			else 
				m_interacted = false;
			ImGui::Image((ImTextureID)m_buffer->GetTextureId(), size, ImVec2(0,1), ImVec2(1,0));

			Drop();

			ImGui::SetCursorPos(cursorPos);
			DrawHelperBar();

			if (HierarchyInterface::s_SelectedEntity) {
				std::shared_ptr<Entity> selectedEntity = HierarchyInterface::s_SelectedEntity;
				auto transform = selectedEntity->GetTransform();

				glm::mat4 worldMatrix = transform->GetLocalToWorldMatrix();

				Renderer::DisableDepth();
				ImGuizmo::SetRect(cursorScreenPos.x, cursorScreenPos.y, (float)m_windowSize.x, (float)m_windowSize.y);
				ImGuizmo::SetDrawlist();
				if (ImGuizmo::Manipulate(&m_camera->GetCamera()->GetViewMatrix()[0][0], &m_camera->GetCamera()->GetProjectionMatrix()[0][0], (ImGuizmo::OPERATION)m_gizmoOperation, (ImGuizmo::MODE)m_gizmoMode, &worldMatrix[0][0])) {
					transform->SetWorldMatrix(worldMatrix);
				}
				Renderer::EnableDepth();
			}

		}

		
		ImGui::End();
	}

	void SceneInterface::StartScene()
	{
		m_buffer->Bind();

		ivec2 textureSize = ivec2(m_buffer->GetWidth(), m_buffer->GetHeight());
		Renderer::SetViewport(0, 0, (unsigned int)m_windowSize.x, (unsigned int)m_windowSize.y);
		if (m_windowSize.x != textureSize.x || m_windowSize.y != textureSize.y) {
			m_camera->GetCamera()->SetViewport(0, 0, m_windowSize.x, m_windowSize.y);
			m_buffer->Resize(m_windowSize.x, m_windowSize.y);
		}

		m_buffer->Clear();	
	}

	void SceneInterface::EndScene()
	{
		m_buffer->Unbind();
	}

	void SceneInterface::Drop()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_EXPLORER_FILE"))
			{
				std::string path = (const char*)payload->Data;
				if (TextureImporter::CheckIfIsImage(path.c_str())) {
					ChargeTexture(path);
				}
				else if (MeshImporter::CheckIfIsModel(path.c_str())) {
					ChargeModel(path);
				}
				else if (MaterialImporter::CheckIfIsMaterial(path.c_str())) {
					ChargeMaterial(path);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
	void SceneInterface::DrawHelperBar()
	{
		
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));

		int currentMode = (m_gizmoMode == ImGuizmo::WORLD) ? 0 : 1;
		ImGui::SetNextItemWidth(65);
		if (ImGui::Combo("##mode", &currentMode, m_gizmoModes.data(), (int)m_gizmoModes.size()))
			m_gizmoMode = (currentMode == 0) ? (int)ImGuizmo::WORLD : (int)ImGuizmo::LOCAL;

		if (ImGui::ImageButton("move",(ImTextureID)m_moveIcon->GetRendererId(), ImVec2(15, 15)))
			m_gizmoOperation = (int)ImGuizmo::TRANSLATE;

		if (ImGui::ImageButton("rotate", (ImTextureID)m_rotateIcon->GetRendererId(), ImVec2(15, 15)))
			m_gizmoOperation = (int)ImGuizmo::ROTATE;

		if (ImGui::ImageButton("scale", (ImTextureID)m_scaleIcon->GetRendererId(), ImVec2(15, 15)))
			m_gizmoOperation = (int)ImGuizmo::SCALE;

		if (ImGui::ImageButton("all", (ImTextureID)m_trsIcon->GetRendererId(), ImVec2(15, 15)))
			m_gizmoOperation = (int)ImGuizmo::UNIVERSAL;

		ImGui::PopStyleVar(2);
	}

	void SceneInterface::ChargeModel(const std::string& modelPath)
	{
		Metadata& meta = AssetRegistry::GetOrCreateMetadata(modelPath);
		MeshImporter::ImportModel(modelPath, meta);
		std::shared_ptr<Entity> parent;

		if (meta.CachesPath.size() > 0) {
			parent = Application::GetInstance().GetScene().CreateEntity("ModelEntity", HierarchyInterface::s_SelectedEntity);
		}
		for (size_t i = 0; i < meta.CachesPath.size(); i++)
		{			
			std::shared_ptr<Mesh> mesh = ResourceManager::GetMesh(meta, (int)i);
			if (mesh) {
				std::shared_ptr<Entity> newEntity;
				if (!parent) {
					newEntity = Application::GetInstance().GetScene().CreateEntity(mesh->GetData().Name,HierarchyInterface::s_SelectedEntity);
				}else
					newEntity = Application::GetInstance().GetScene().CreateEntity(mesh->GetData().Name, parent);

				MeshRenderer* renderer = newEntity->AddComponent<MeshRenderer>();
				renderer->SetMesh(mesh);
				renderer->GetTransform()->SetLocalPosition(mesh->GetData().Position);
				renderer->GetTransform()->SetLocalRotation(mesh->GetData().Rotation);
				renderer->GetTransform()->SetLocalScale(mesh->GetData().Scale);
			}
		}
	}
	void SceneInterface::ChargeTexture(const std::string& texturePath)
	{
		Metadata& meta = AssetRegistry::GetOrCreateMetadata(texturePath);

		TextureImporter::ImportImage(texturePath, meta);
		std::shared_ptr<Texture> texture = ResourceManager::GetTexture(meta);
		if (texture) {
			if (HierarchyInterface::s_SelectedEntity != nullptr) {
				MeshRenderer* renderer = HierarchyInterface::s_SelectedEntity->GetComponent<MeshRenderer>();
				if (renderer) {
					if (renderer->GetMaterial())
						renderer->GetMaterial()->SetTexture(texture);
				}
			}
			else {
				for (const auto& [uuid, entity] : Application::GetInstance().GetScene().GetAllEntities())
				{
					MeshRenderer* renderer = entity->GetComponent<MeshRenderer>();
					if (renderer) {
						if(renderer->GetMaterial())
							renderer->GetMaterial()->SetTexture(texture);
					}
				}
			}
		}
	}
	void SceneInterface::ChargeMaterial(const std::string& materialPath)
	{
		Metadata& meta = AssetRegistry::GetOrCreateMetadata(materialPath);

		MaterialImporter::ImportMaterial(materialPath, meta);
		std::shared_ptr<Material> material = ResourceManager::GetMaterial(meta);
		if (material) {
			if (HierarchyInterface::s_SelectedEntity != nullptr) {
				MeshRenderer* renderer = HierarchyInterface::s_SelectedEntity->GetComponent<MeshRenderer>();
				if (renderer) {
					renderer->SetMaterial(material);
				}
			}
			else {
				for (const auto& [uuid, entity] : Application::GetInstance().GetScene().GetAllEntities())
				{
					MeshRenderer* renderer = entity->GetComponent<MeshRenderer>();
					if (renderer) {
						renderer->SetMaterial(material);
					}
				}
			}
		}
	}
}