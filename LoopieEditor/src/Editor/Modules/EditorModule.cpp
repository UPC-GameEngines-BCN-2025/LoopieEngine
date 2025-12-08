#include "EditorModule.h"

#include "Loopie/Core/Application.h"

//// Test
#include "Loopie/Core/Log.h"
#include "Loopie/Render/Renderer.h"
#include "Loopie/Render/Gizmo.h"

#include "Loopie/Math/MathTypes.h"

#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Importers/TextureImporter.h"
#include "Loopie/Importers/MaterialImporter.h"

#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Resources/Types/Material.h"
///

#include <glad/glad.h>

namespace Loopie
{
	void EditorModule::OnLoad()
	{
		AssetRegistry::Initialize();
		Application::GetInstance().GetWindow().SetResizable(true);

		/////SCENE
		Application::GetInstance().CreateScene(""); /// Maybe default One
		scene = &Application::GetInstance().GetScene();
		CreateBakerHouse();

		scene->CreateEntity({ 0,0,-10 }, { 1,0,0,0 }, {1,1,1}, nullptr, "MainCamera")->AddComponent<Camera>();
		scene->CreateEntity({ 0,0,-20 }, { 1,0,0,0 }, {1,1,1}, nullptr, "SecondaryCamera")->AddComponent<Camera>();

		Metadata& metadata = AssetRegistry::GetOrCreateMetadata("assets/materials/outlineMaterial.mat");
		selectedObjectMaterial = ResourceManager::GetMaterial(metadata);
		selectedObjectMaterial->SetIfEditable(false);
		selectedObjectShader = new Shader("assets/shaders/SelectionOutline.shader");
		selectedObjectMaterial->SetShader(*selectedObjectShader);

		////

		m_assetsExplorer.Init();
		m_console.Init();
		m_hierarchy.Init();
		m_inspector.Init();
		m_game.Init();
		m_scene.Init();
		m_mainMenu.Init();

		m_hierarchy.SetScene(scene);

		Application::GetInstance().m_notifier.AddObserver(this);

	}

	void EditorModule::OnUnload()
	{
		AssetRegistry::Shutdown();
		Application::GetInstance().m_notifier.RemoveObserver(this);
	}

	void EditorModule::OnUpdate()
	{

		Application& app = Application::GetInstance();
		InputEventManager& inputEvent = app.GetInputEvent();

		m_hierarchy.Update(inputEvent);
		m_assetsExplorer.Update(inputEvent);
		m_scene.Update(inputEvent);

		const std::vector<Camera*>& cameras = Renderer::GetRendererCameras();
		for (const auto cam : cameras)
		{
			std::shared_ptr<FrameBuffer> buffer = cam->GetRenderTarget();
			if (buffer)
			{
				buffer->Bind();
				buffer->Clear();
				buffer->Unbind();
			}
		}

		/// RenderToTarget
		for (const auto cam : cameras)
		{
			if (!cam->GetIsActive())
				continue;

			std::shared_ptr<FrameBuffer> buffer = cam->GetRenderTarget();
			if (!buffer)
				continue;

			Renderer::BeginScene(cam->GetViewMatrix(), cam->GetProjectionMatrix(), false);
			Renderer::SetViewport(0, 0, buffer->GetWidth(), buffer->GetWidth());
			buffer->Bind();
			RenderWorld(cam);
			Renderer::EndScene();

			if (buffer) {
				buffer->Unbind();
			}
		}

		/// SceneWindowRender
		m_scene.StartScene();
		Renderer::BeginScene(m_scene.GetCamera()->GetViewMatrix(), m_scene.GetCamera()->GetProjectionMatrix(), true);
		RenderWorld(m_scene.GetCamera());
		if (HierarchyInterface::s_SelectedEntity) {
			Camera* cam = HierarchyInterface::s_SelectedEntity->GetComponent<Camera>();
			if (cam)
				Gizmo::DrawFrustum(cam->GetFrustum());
		}
		Renderer::EndScene();
		m_scene.EndScene();

		/// GameWindowRender
		m_game.StartScene();
		if (m_game.GetCamera() && m_game.GetCamera()->GetIsActive()) {
			Renderer::BeginScene(m_game.GetCamera()->GetViewMatrix(), m_game.GetCamera()->GetProjectionMatrix(), false);
			RenderWorld(m_game.GetCamera());
			Renderer::EndScene();
		}
		m_game.EndScene();
	}

	void EditorModule::OnInterfaceRender()
	{
		ImGui::DockSpaceOverViewport();

		m_mainMenu.Render();
		m_inspector.Render();
		m_console.Render();
		m_hierarchy.Render();
		m_assetsExplorer.Render();
		m_game.Render();
		m_scene.Render();
	}

	void EditorModule::RenderWorld(Camera* camera)
	{	
		Renderer::EnableStencil();
		Renderer::EnableDepth();
		Renderer::Clear();
		Renderer::SetStencilFunc(Renderer::StencilFunc::ALWAYS, 1, 0xFF);
		Renderer::SetStencilOp(Renderer::StencilOp::KEEP, Renderer::StencilOp::KEEP, Renderer::StencilOp::REPLACE);
		Renderer::SetStencilMask(0xFF);
		//Renderer::DisableStencil();
		for (auto& [uuid, entity] : scene->GetAllEntities()) {
			if (!entity->GetIsActive())
				continue;
			MeshRenderer* renderer = entity->GetComponent<MeshRenderer>();
			if (!renderer || !renderer->GetIsActive())
				continue;

			if (!camera->GetFrustum().Intersects(renderer->GetWorldAABB()))
				continue;
			renderer->Render();

			if (entity == HierarchyInterface::s_SelectedEntity) {
				Renderer::FlushRenderItem(renderer->GetMesh()->GetVAO(), renderer->GetMaterial(), entity->GetTransform());

				Renderer::SetStencilFunc(Renderer::StencilFunc::NOTEQUAL,1,0xFF);
				Renderer::SetStencilMask(0x00);

				float outlineScale = 1.05f;
				glm::vec3 center = renderer->GetWorldAABB().GetCenter();

				glm::mat4 T1 = glm::translate(glm::mat4(1.0f), center);
				glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(outlineScale));
				glm::mat4 T2 = glm::translate(glm::mat4(1.0f), -center);
				glm::mat4 outlineModel = T1 * S * T2 * entity->GetTransform()->GetLocalToWorldMatrix();
				Renderer::FlushRenderItem(renderer->GetMesh()->GetVAO(), selectedObjectMaterial, outlineModel);

				Renderer::SetStencilMask(0xFF);
				Renderer::EnableDepth();
				Renderer::DisableStencil();

			}
			else
			{
				Renderer::AddRenderItem(renderer->GetMesh()->GetVAO(), renderer->GetMaterial(), entity->GetTransform());
			}
		}
	}

	void EditorModule::CreateBakerHouse()
	{
		m_scene.ChargeModel("assets/models/BakerHouse.fbx");
		m_scene.ChargeTexture("assets/textures/Baker_house.png");
	}

	void EditorModule::OnNotify(const EngineNotification& type)
	{
		if (type == EngineNotification::OnProjectChange) {
			AssetRegistry::Initialize();
			Application::GetInstance().GetWindow().SetTitle(Application::GetInstance().m_activeProject.GetProjectName().c_str());
			m_assetsExplorer.Reload();
			///LOAD SCENE
		}
	}

}