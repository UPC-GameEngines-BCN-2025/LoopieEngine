#include "ImGuiManager.h"

#include "Loopie/Core/Application.h"
#include "Loopie/Core/Window.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

namespace Loopie {
	ImGuiManager::~ImGuiManager()
	{
		ImGui_ImplSDL3_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiManager::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		Window* window = Application::GetInstance().GetWindow();
		ImGui_ImplSDL3_InitForOpenGL(window->GetSDLWindow(), window->GetSDL_GL_Context());
		ImGui_ImplOpenGL3_Init();
	}
	void ImGuiManager::GetEvents(const SDL_Event& event)
	{
		ImGui_ImplSDL3_ProcessEvent(&event);
	}
	void ImGuiManager::StartFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
	}
	void ImGuiManager::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}