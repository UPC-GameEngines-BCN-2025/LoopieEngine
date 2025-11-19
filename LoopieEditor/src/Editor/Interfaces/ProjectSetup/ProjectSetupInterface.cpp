#include "ProjectSetupInterface.h"

#include "Loopie/Files/FileDialog.h"
#include "Loopie/Files/DirectoryManager.h"


#include "Loopie/Core/Application.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Core/Window.h"
#include "Loopie/Files/Json.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <cstddef>

namespace Loopie
{
	ProjectSetupInterface::ProjectSetupInterface()
	{
		/*Application::GetInstance().GetWindow()->SetTitle("Loopie Engine | Project Setup");
		Application::GetInstance().GetWindow()->SetResizable(false);
		windowSizeX = Application::GetInstance().GetWindow()->GetSize().first;
		windowSizeY = Application::GetInstance().GetWindow()->GetSize().second;*/

		if (!DirectoryManager::Contains("recentProjects.json")) {
			Json::WriteToFileFromString("recentProjects.json", R"({"recent" : []})");
		}

		GetRecentProjects();
	}

	ProjectSetupInterface::~ProjectSetupInterface() {

		JsonData data = Json::ReadFromFile("recentProjects.json");

		JsonNode recentArray = data.Root().Child("recent");
		recentArray.ClearArray();

		for (size_t i = 0; i < m_recentProjects.size(); i++)
		{
			recentArray.AddArrayElement<std::string>(m_recentProjects[i]);
		}

		data.ToFile("recentProjects.json");
	}

	void ProjectSetupInterface::Render()
	{
		RenderRecentProjectsMenu();
		RenderLoadProjectMenu();
		RenderCreateProjectMenu();
	}
	void ProjectSetupInterface::GetRecentProjects()
	{
		JsonData data = Json::ReadFromFile("recentProjects.json");
		if (data.IsEmpty())
			return;
		JsonNode recentArray = data.Root().Child("recent");
		if (recentArray.IsArray()) {
			JsonResult<std::string> result;
			for (unsigned int i = 0; i < recentArray.Size(); ++i) {
				result = recentArray.GetArrayElement<std::string>(i);

				m_recentProjects.emplace_back(result.Result);
			}
		}

	}

	bool ProjectSetupInterface::ExistsRecentProject(const std::string& id)
	{
		auto it = std::find(m_recentProjects.begin(), m_recentProjects.end(), id);
		if (it != m_recentProjects.end()) {
			return true;
		}
		return false;
	}

	void ProjectSetupInterface::DeleteRecentProject(const std::string& id)
	{
		auto it = std::find(m_recentProjects.begin(), m_recentProjects.end(), id);
		if (it != m_recentProjects.end()) {
			m_recentProjects.erase(it);
		}
	}

	bool ProjectSetupInterface::TryOpenProject(const std::string& path) const
	{
		if (!DirectoryManager::Contains(path))
			return false;
		if(!Application::GetInstance().m_activeProject.Open(path))
			return false;

		return true;
	}

	void ProjectSetupInterface::RenderRecentProjectsMenu()
	{
		ImGui::Begin("Recent Projects", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		for (size_t i = 0; i < m_recentProjects.size(); i++)
		{
			ImGui::PushID((int)(i));

			ImGui::BeginGroup();

			ImGui::InputText("##ProjectPath", &m_recentProjects[i], ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("Open")) {
				TryOpenProject(m_recentProjects[i]);
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				DeleteRecentProject(m_recentProjects[i]);
				ImGui::EndGroup();
				ImGui::PopID();
				break;
			}

			ImGui::EndGroup();

			ImGui::PopID();
		}

		ImGui::End();
	}
	void ProjectSetupInterface::RenderCreateProjectMenu() 
	{
		ImGui::Begin("Create Project", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::InputText("Project Name", m_projectName, IM_ARRAYSIZE(m_projectName));

		ImGui::InputText("Path", &m_createProjectPath, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("##", { 20,20 }))
		{
			DialogResult result = FileDialog::SelectFolder();
			if (result.Status == DialogResultType::SUCCESS)
			{
				m_createProjectPath = result.Paths[0].string();
			}
		}

		if (ImGui::Button("Create Project", { 150,20 }))
		{
			if (Application::GetInstance().m_activeProject.Create(m_createProjectPath, m_projectName)) {

				std::filesystem::path path = m_createProjectPath;
				path /= m_projectName;

				if (!ExistsRecentProject(path.string())) {
					m_recentProjects.emplace_back(path.string());
				}
			}
		}
		ImGui::End();
	}
	void ProjectSetupInterface::RenderLoadProjectMenu()
	{
		ImGui::Begin("Load Project", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::InputText("Path", &m_loadProjectPath, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("##", { 20,20 }))
		{
			DialogResult result = FileDialog::SelectFolder();
			if (result.Status == DialogResultType::SUCCESS)
			{
				m_loadProjectPath = result.Paths[0].string();
			}
		}

		if (ImGui::Button("Open Project", { 150,20 }))
		{
			if (Application::GetInstance().m_activeProject.Open(m_loadProjectPath)) {
				if (!ExistsRecentProject(m_loadProjectPath)) {
					m_recentProjects.emplace_back(m_loadProjectPath);
				}
			}
		}
		ImGui::End();
	}
}

