#include "Application.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Core/Log.h"

#include <SDL3/SDL_init.h> // TEMP INCLUDE FOR POLLING EVENTS
#include <SDL3/SDL.h>// TEMP INCLUDE FOR POLLING EVENTS
#include <imgui.h>// TEMP INCLUDE FOR POLLING EVENTS


#include "Loopie/Files/FileDialog.h"

namespace Loopie {
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		Log::Init();
		Log::Info("Starting Application...");

		ASSERT(s_Instance != nullptr, "Application is already created");
		s_Instance = this;

		Log::Info("Application Started");

		// Window Creation
		m_window = new Window();
		Log::Info("Window created successfully.");

		m_imguiManager.Init();

	}

	Application::~Application()
	{
		Log::Info("Closing Application...");

		for (Module* module : m_modules)
		{
			module->OnUnload();
			delete(module);
		}
		m_modules.clear();

		//// Cleaning
		delete(m_window); 
		m_window = nullptr;

		Log::Info("Application Closed");
	}

	void Application::AddModule(Module* module)
	{
		m_modules.emplace_back(module);
		module->OnLoad();
		
	}

	void Application::RemoveModule(Module* module)
	{
		auto it = std::find(m_modules.begin(), m_modules.end(), module);
		if (it == m_modules.end())
		{
			if (module)
				Log::Warn("Module to remove not found -> {0}", module->GetName());
			else
				Log::Error("Module to remove is null");
			return;		
		}	
		module->OnUnload();
		m_modules.erase(it);
	}

	Application& Application::GetInstance()
	{
		return *s_Instance;
	}

	Window* Application::GetWindow() const
	{
		return m_window;
	}

	void Application::Run()
	{
		while (m_running)
		{

			m_window->ClearWindow(); ///Test -> this should be moved to a RenderClass in the future
			
			m_imguiManager.StartFrame();

			ProcessEvents();

			for (Module* module : m_modules) {
				if (module->IsActive()) {
					module->OnUpdate();
				}
			}

			ImGui::ShowDemoWindow();
			m_imguiManager.EndFrame();

			m_window->Update();		
		}
	}

	void Application::ProcessEvents()
	{
		// TEMP TESTING POLLING! -> Requiered by ImGui
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			m_imguiManager.GetEvents(e);
			if (e.type == SDL_EVENT_QUIT)
			{
				Close();
				break; // Exit the loop
			}
			else if (e.type == SDL_EVENT_KEY_DOWN)
			{
				// USED FOR WINDOW TESTING RIGHT NOW! (Aware that switch case would be better but testing laziness.)
				SDL_Keycode key = e.key.key;
				if (key == SDLK_F1)
				{
					m_window->GetSize().second, m_window->GetPosition().first, m_window->GetPosition().second, m_window->IsFullscreen();
				}
				else if (key == SDLK_F2)
				{
					m_window->SetWindowFullscreen(!m_window->IsFullscreen());
				}
				else if (key == SDLK_F3)
				{
					m_window->SetResizable(true);
				}
				else if (key == SDLK_F4)
				{
					m_window->SetResizable(false);
				}
				else if (key == SDLK_F5)
				{
					m_window->SetTitle("Loopie!");
				}
				else if (key == SDLK_F6)
				{
					m_window->SetPosition(10, 10);
				}
				else if (key == SDLK_F7)
				{
					m_window->SetWindowSize(1280, 720);
				}
				else if (key == SDLK_F8)
				{
					m_window->SetWindowSize(1280, 720, true);
				}
				else if (key == SDLK_ESCAPE)
				{
					Close();
				}
			}
		}
		// TEMP TESTING POLLING OVER!
	}

	void Application::Close()
	{
		m_running = false;
	}
}
