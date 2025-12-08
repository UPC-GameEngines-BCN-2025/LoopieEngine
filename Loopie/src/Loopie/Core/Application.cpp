#include "Application.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Core/Time.h"
#include "Loopie/Render/Renderer.h"

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
		delete module;
	}

	Application& Application::GetInstance()
	{
		return *s_Instance;
	}

	Window& Application::GetWindow()
	{
		return *m_window;
	}

	InputEventManager& Application::GetInputEvent()
	{
		return m_inputEvent;
	}

	Scene& Application::GetScene()
	{
		return *m_scene;
	}

	void Application::Run()
	{
		while (m_running)
		{

			Renderer::Clear();

			Time::CalculateFrame();

			m_imguiManager.StartFrame();

			m_inputEvent.Update();

			m_window->ProcessEvents(m_inputEvent);
			ProcessEvents(m_inputEvent);

			for (Module* module : m_modules) {
				if (module->IsActive()) {
					module->OnUpdate();
				}
			}

			if(m_renderInterface){
				for (Module* module : m_modules) {
					if (module->IsActive()) {
						module->OnInterfaceRender();
					}
				}
			}

			m_imguiManager.EndFrame();

			m_window->Update();		
		}
	}

	void Application::CreateScene(const std::string& filePath)
	{
		m_scene = new Scene(filePath);
	}

	void Application::LoadScene(const std::string& filePath)
	{
		delete m_scene;
		m_scene = new Scene(filePath);
	}

	void Application::ProcessEvents(InputEventManager& eventController)
	{
		if (eventController.HasEvent(SDL_EVENT_QUIT))
		{
			Close();
			return;
		}
		if (eventController.HasEvent(SDL_EVENT_KEY_DOWN)) {
			if (eventController.GetKeyStatus(SDL_SCANCODE_F11) == KeyState::DOWN)
			{
				m_window->SetWindowFullscreen(!m_window->IsFullscreen());
			}
			else if (eventController.GetKeyStatus(SDL_SCANCODE_ESCAPE) == KeyState::DOWN)
			{
				Close();
			}
		}
	}

	void Application::Close()
	{
		m_running = false;
	}
}
