#pragma once

#include "Loopie/Core/Module.h"
#include "Loopie/Core/Window.h"
#include "Loopie/Core/InputEventManager.h"
#include "Loopie/Events/Event.h"
#include "Loopie/Events/EventTypes.h"
#include "Loopie/Project/Project.h"
#include "Loopie/ImGui/ImGuiManager.h"
#include "Loopie/Scene/Scene.h"

#include <vector>

namespace Loopie {
	class Application {
	public:
		Application();
		~Application();

		void AddModule(Module* module);
		void RemoveModule(Module* module);

		void Run();
		void Close();

		static Application& GetInstance();
		Window& GetWindow();
		InputEventManager& GetInputEvent();
		Scene& GetScene();

		void SetInterfaceState(bool enabled) { m_renderInterface = enabled; }
		bool IsInterfaceVisible() const { return m_renderInterface; }

		void CreateScene(const std::string& filePath);
		void LoadScene(const std::string& filePath);

	private:
		void ProcessEvents(InputEventManager& eventController);

	public:
		Project m_activeProject;
		Event<EngineNotification> m_notifier;
	private:
		static Application* s_Instance;
		Scene* m_scene = nullptr;

		std::vector<Module*> m_modules;
		Window* m_window = nullptr;

		ImGuiManager m_imguiManager;

		InputEventManager m_inputEvent;

		bool m_running = true;
		bool m_renderInterface = true;

		
	};

	//// Define Main
	/*int main() {
		Application* app = new "ApplicationType()";
		app->Run();
		delete app;
	}*/
}