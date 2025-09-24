#pragma once

#include "Loopie/Core/Module.h"
#include "Loopie/Core/Window.h"
#include "Loopie/ImGui/ImGuiManager.h"

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
		Window* GetWindow() const;

	private:
		void ProcessEvents();

	private:
		static Application* s_Instance;

		std::vector<Module*> m_modules;
		Window* m_window = nullptr;

		ImGuiManager m_imguiManager;

		bool m_running = true;
	};

	//// Define Main
	/*int main() {
		Application* app = new "ApplicationType()";
		app->Run();
		delete app;
	}*/
}