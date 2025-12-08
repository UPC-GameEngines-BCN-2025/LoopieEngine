#include "EditorMenuInterface.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Core/Time.h"
#include "Loopie/Core/Window.h"
#include "Loopie/Files/FileDialog.h"

#include <imgui.h>
#include <imgui_stdlib.h>
#include <SDL3/SDL_misc.h>
#include <SDL3/SDL_cpuinfo.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_gpu.h>

namespace Loopie {
	EditorMenuInterface::EditorMenuInterface() {
	}

	void EditorMenuInterface::Render() {

		ImGuiID openProjectPopUpId = ImGui::GetID("###OpenProjectPopUp");
		ImGuiID createProjectPopUpId = ImGui::GetID("###CreateProjectPopUp");

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open"))
				{
					ImGui::OpenPopup(openProjectPopUpId);
					m_projectPath = "";
				}

				if (ImGui::MenuItem("New"))
				{
					ImGui::OpenPopup(createProjectPopUpId);
					m_projectPath = "";
					m_projectName[0] = '\0';
				}

				if (ImGui::MenuItem("Save [WIP]"))
				{
					// TODO: save all scenes and config
					Application::GetInstance().GetScene().SaveScene();
				}

				//if (ImGui::MenuItem("Save as... [WIP]"))
				//{
				//	// TODO: save all scenes and config to a specific file
				//	Application::GetInstance().GetScene().SaveScene();
				//}

				if (ImGui::MenuItem("Reload Scene... [WIP][TEMP]"))
				{
					Application::GetInstance().GetScene().ReadAndLoadSceneFile("TESTSavedScene.json");
				}

				if (ImGui::MenuItem("Exit"))
					Application::GetInstance().Close();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Disable"))
				{
					Application::GetInstance().SetInterfaceState(false);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Info"))
			{

				if (ImGui::MenuItem("Configuration"))
				{
					m_showInfoConfigMenu = true;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Documentation"))
				{
					SDL_OpenURL("https://github.com/UPC-GameEngines-BCN-2025/LoopieEngine/docs");
				}

				if (ImGui::MenuItem("Report"))
				{
					SDL_OpenURL("https://github.com/UPC-GameEngines-BCN-2025/LoopieEngine/issues");
				}

				if (ImGui::MenuItem("Release"))
				{
					SDL_OpenURL("https://github.com/UPC-GameEngines-BCN-2025/LoopieEngine/releases");
				}

				if (ImGui::MenuItem("About"))
				{
					m_showAboutMenu = true;
				}
				ImGui::EndMenu();
			}


			ImGui::EndMainMenuBar();
		}

		if (m_showAboutMenu)
			RenderAboutMenu();

		if (m_showInfoConfigMenu)
			RenderInfoConfigMenu();

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		RenderOpenProjectPopUp();

		ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		RenderCreateProjectPopUp();
		
	}

	void EditorMenuInterface::RenderAboutMenu()
	{

		ImGui::Begin("About Loopie Engine", &m_showAboutMenu, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

		ImGui::Text("Loopie Engine v0.0.1");
		ImGui::Separator();
		ImGui::Text("Developed by:");
		ImGui::BulletText("Ana Alcazar");
		ImGui::BulletText("Pol Sierra");
		ImGui::BulletText("Adria Garcia");

		ImGui::Separator();
		ImGui::Text("Built using:");
		ImGui::BulletText("SDL3 / OpenGl / Glm");
		ImGui::BulletText("Dear ImGui / ImGuizmo");
		ImGui::BulletText("Assimp / Spdlog / Devil");
		ImGui::BulletText("Nativefiledialog-Extended");
		ImGui::BulletText("nlohmann-json");

		ImGui::Separator();
		ImGui::BeginChild("LicenseText",{300,150});
		ImGui::TextWrapped(
			"MIT License\n"
			"Copyright (c) 2025 CITM - UPC\n\n"
			"Permission is hereby granted, free of charge, to any person obtaining a copy "
			"of this software and associated documentation files (the \"Software\"), to deal "
			"in the Software without restriction, including without limitation the rights "
			"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
			"copies of the Software, and to permit persons to whom the Software is "
			"furnished to do so, subject to the following conditions:\n\n"
			"The above copyright notice and this permission notice shall be included in all "
			"copies or substantial portions of the Software.\n\n"
			"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
			"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
			"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
			"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
			"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
			"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
			"SOFTWARE.");
		ImGui::EndChild();

		ImGui::End();
	}

	void EditorMenuInterface::RenderInfoConfigMenu() {
		

		ImGui::Begin("Configuration", &m_showInfoConfigMenu, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

		Window& window = Application::GetInstance().GetWindow();

		if (ImGui::CollapsingHeader("Application")) {
			ImGui::Text("Engine: LoopieEngine");
			ImGui::Text("Organization: LoopieStudios");
		}

		if (ImGui::CollapsingHeader("Window")) {

			bool windowFullscreen = window.IsFullscreen();
			if (ImGui::Checkbox("Fullscreen", &windowFullscreen)) {
				window.SetWindowFullscreen(windowFullscreen);
			}
			bool windowVsync = window.IsVsyncEnabled();
			if (ImGui::Checkbox("VSync", &windowVsync)) {
				window.SetVsync(windowVsync);
			}
			if (!windowVsync) {
				int frameRate = window.GetFramerateLimit();
				int minFrameRate = 10;
				int maxFrameRate = 9999;

				if (ImGui::SliderInt("FrameRate", &frameRate, minFrameRate, maxFrameRate,"%d", ImGuiSliderFlags_Logarithmic)) {
					window.SetFramerateLimit(frameRate);
				}
			}


			float ms = Time::GetDeltaTimeMs();
			float fps = ms > 0.0f ? 1000.0f / ms : 0.0f;

			if (m_fpsLog.size() >= LOG_SIZE)
				m_fpsLog.erase(m_fpsLog.begin());
			m_fpsLog.push_back(fps);

			if (m_msLog.size() >= LOG_SIZE)
				m_msLog.erase(m_msLog.begin());
			m_msLog.push_back(ms);

			char title[25];
			snprintf(title, 25, "Framerate %.1f", m_fpsLog.back());
			ImGui::PlotHistogram("##framerate", &m_fpsLog[0], (int)m_fpsLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			snprintf(title, 25, "Milliseconds %.1f", m_msLog.back());
			ImGui::PlotHistogram("##milliseconds", &m_msLog[0], (int)m_msLog.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

		}

		if (ImGui::CollapsingHeader("Hardware Info")) {		
			ImGui::Text("CPUs: %d", SDL_GetNumLogicalCPUCores());
			ImGui::Text("Cache: %dkb", SDL_GetCPUCacheLineSize());

			ImGui::Text("System RAM: %.1f GB", SDL_GetSystemRAM() / 1024.0f);

			std::string caps = "Caps: ";
			if (SDL_HasAltiVec()) caps += "MW,";
			if (SDL_HasMMX()) caps += "MMX,";
			if (SDL_HasSSE()) caps += "SSE,";
			if (SDL_HasSSE2()) caps += "SSE2,";
			if (SDL_HasSSE3()) caps += "SSE3,";
			if (SDL_HasSSE41()) caps += "SSE41,";
			if (SDL_HasSSE42()) caps += "SSE42,";
			if (SDL_HasAVX()) caps += "AVX,";
			if (SDL_HasAVX2()) caps += "AVX2,";
			if (SDL_HasAVX512F()) caps += "AVX512,";

			if (!caps.empty() && caps.back() == ',') caps.pop_back();
			ImGui::Text("%s", caps.c_str());

		}

		ImGui::InvisibleButton("##", { 310,1 });
		ImGui::End();
	}

	void EditorMenuInterface::RenderOpenProjectPopUp()
	{
		if (ImGui::BeginPopupModal("Open Project###OpenProjectPopUp", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("Path",& m_projectPath, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("##", { 20,20 }))
			{
				DialogResult result = FileDialog::SelectFolder();
				if (result.Status == DialogResultType::SUCCESS)
				{
					m_projectPath = result.Paths[0].string();
				}
			}

			if (ImGui::Button("Open Project", { 150,20 }))
			{
				if (Application::GetInstance().m_activeProject.Open(m_projectPath)) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void EditorMenuInterface::RenderCreateProjectPopUp() {
		if (ImGui::BeginPopupModal("Create Project###CreateProjectPopUp", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {

			ImGui::InputText("Project Name", m_projectName, IM_ARRAYSIZE(m_projectName));

			ImGui::InputText("Path", &m_projectPath, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("##", { 20,20 }))
			{
				DialogResult result = FileDialog::SelectFolder();
				if (result.Status == DialogResultType::SUCCESS)
				{
					m_projectPath = result.Paths[0].string();
				}
			}

			if (ImGui::Button("Create Project", { 150,20 }))
			{
				if (Application::GetInstance().m_activeProject.Create(m_projectPath, m_projectName)) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}