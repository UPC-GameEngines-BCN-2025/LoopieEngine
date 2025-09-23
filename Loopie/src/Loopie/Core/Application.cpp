#include "Application.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Core/Log.h"


namespace Loopie {
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		Log::Init();

		Log::Info("Starting Application...");

		ASSERT(s_Instance != nullptr, "Application is already created");
		s_Instance = this;

		Log::Info("Application Started");
	}

	Application::~Application()
	{
		Log::Info("Closing Application...");

		//// Cleaning

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

	void Application::Run()
	{
		while (m_running)
		{
			for (Module* module : m_modules) {
				if (module->IsActive()) {
					module->OnUpdate();
				}
			}
		}
	}

	void Application::Close()
	{
		m_running = false;
	}
}
