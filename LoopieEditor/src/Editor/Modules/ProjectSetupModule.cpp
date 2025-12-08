#include "ProjectSetupModule.h"

#include "EditorModule.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Project/Project.h"

#include <imgui.h>


namespace Loopie
{
	void ProjectSetupModule::OnUpdate()
	{
		Application& app = Application::GetInstance();
		if (!app.m_activeProject.IsEmpty())
		{
			Application::GetInstance().GetWindow().SetTitle(app.m_activeProject.GetProjectName().c_str());
			app.AddModule(new EditorModule());
			app.RemoveModule(this);
		}
	}
	void ProjectSetupModule::OnLoad()
	{
		m_interface = new ProjectSetupInterface();
		Application::GetInstance().GetWindow().SetTitle("Loopie Engine | Project Setup");
		Application::GetInstance().GetWindow().SetResizable(false);
	}
	void ProjectSetupModule::OnUnload()
	{
		delete m_interface;
	}
	void ProjectSetupModule::OnInterfaceRender()
	{
		ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
		ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport(), ImGuiDockNodeFlags_NoResize);
		m_interface->Render();
	}
}