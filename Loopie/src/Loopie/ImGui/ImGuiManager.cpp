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
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		Window* window = Application::GetInstance().GetWindow();
		ImGui_ImplSDL3_InitForOpenGL(window->GetSDLWindow(), window->GetSDL_GL_Context());
		ImGui_ImplOpenGL3_Init();

		CustomImGui();
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
		Window* window = Application::GetInstance().GetWindow();
		std::pair<int, int> size = window->GetSize();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)size.first, (float)size.second);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{

			SDL_Window* backup_window = window->GetSDLWindow();
			SDL_GLContext backup_context = window->GetSDL_GL_Context();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			SDL_GL_MakeCurrent(backup_window, backup_context);
		}
	}

    void ImGuiManager::CustomImGui()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        // ==============================
        // STYLE SETTINGS
        // ==============================
        style.WindowRounding = 6.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 4.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;

        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 10.0f;

        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.TabBorderSize = 1.0f;

        style.ItemSpacing = ImVec2(8, 6);
        style.ItemInnerSpacing = ImVec2(6, 4);
        style.IndentSpacing = 22.0f;
        style.CellPadding = ImVec2(6, 6);

        style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

        // ==============================
        // COLOR VARIABLES (Elegant & Neutral)
        // ==============================
        ImVec4 baseColor = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
        ImVec4 panelColor = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
        ImVec4 panelHighlight = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);

        ImVec4 accentColor = ImVec4(0.70f, 0.65f, 0.50f, 1.00f); // warm subtle accent
        ImVec4 accentHover = ImVec4(0.78f, 0.72f, 0.55f, 1.00f);
        ImVec4 accentActive = ImVec4(0.60f, 0.55f, 0.45f, 1.00f);

        ImVec4 textColor = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
        ImVec4 textSecondary = ImVec4(0.60f, 0.60f, 0.65f, 1.00f);

        ImVec4 borderColor = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
        ImVec4 shadowColor = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);

        ImVec4 popupColor = ImVec4(0.18f, 0.18f, 0.20f, 0.95f);
        ImVec4 menuColor = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);

        ImVec4 tabColor = panelColor;
        ImVec4 tabHoverColor = panelHighlight;
        ImVec4 tabActiveColor = accentColor;

        ImVec4 selectionColor = ImVec4(0.70f, 0.65f, 0.50f, 0.35f);
        ImVec4 dragDropColor = ImVec4(0.90f, 0.85f, 0.40f, 0.85f);

        ImVec4 dockingBg = ImVec4(0.13f, 0.13f, 0.15f, 1.00f);
        ImVec4 dockingPreview = accentColor;

        ImVec4 plotLineColor = ImVec4(0.55f, 0.55f, 0.60f, 1.00f);
        ImVec4 plotLineHover = accentColor;
        ImVec4 plotHistogram = ImVec4(0.70f, 0.65f, 0.50f, 0.70f);
        ImVec4 plotHistogramHover = accentHover;

        // ==============================
        // APPLY COLORS
        // ==============================
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_Text] = textColor;
        colors[ImGuiCol_TextDisabled] = textSecondary;

        colors[ImGuiCol_WindowBg] = baseColor;
        colors[ImGuiCol_ChildBg] = panelColor;
        colors[ImGuiCol_PopupBg] = popupColor;

        colors[ImGuiCol_Border] = borderColor;
        colors[ImGuiCol_BorderShadow] = shadowColor;

        colors[ImGuiCol_FrameBg] = panelColor;
        colors[ImGuiCol_FrameBgHovered] = panelHighlight;
        colors[ImGuiCol_FrameBgActive] = accentActive;

        colors[ImGuiCol_TitleBg] = panelColor;
        colors[ImGuiCol_TitleBgActive] = panelHighlight;
        colors[ImGuiCol_TitleBgCollapsed] = panelColor;

        colors[ImGuiCol_MenuBarBg] = menuColor;
        colors[ImGuiCol_ScrollbarBg] = panelColor;
        colors[ImGuiCol_ScrollbarGrab] = panelHighlight;
        colors[ImGuiCol_ScrollbarGrabHovered] = accentHover;
        colors[ImGuiCol_ScrollbarGrabActive] = accentActive;

        colors[ImGuiCol_CheckMark] = accentColor;
        colors[ImGuiCol_SliderGrab] = accentColor;
        colors[ImGuiCol_SliderGrabActive] = accentHover;

        colors[ImGuiCol_Tab] = tabColor;
        colors[ImGuiCol_TabHovered] = tabHoverColor;
        colors[ImGuiCol_TabActive] = tabActiveColor;
        colors[ImGuiCol_TabUnfocused] = panelColor;
        colors[ImGuiCol_TabUnfocusedActive] = panelHighlight;

        colors[ImGuiCol_DockingPreview] = dockingPreview;
        colors[ImGuiCol_DockingEmptyBg] = dockingBg;

        colors[ImGuiCol_TextSelectedBg] = selectionColor;
        colors[ImGuiCol_DragDropTarget] = dragDropColor;

        colors[ImGuiCol_PlotLines] = plotLineColor;
        colors[ImGuiCol_PlotLinesHovered] = plotLineHover;
        colors[ImGuiCol_PlotHistogram] = plotHistogram;
        colors[ImGuiCol_PlotHistogramHovered] = plotHistogramHover;
    }

}