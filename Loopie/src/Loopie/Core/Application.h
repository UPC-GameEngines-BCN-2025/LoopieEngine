#pragma once

#include "Loopie/Core/Module.h"
#include "Loopie/Core/ImGuiManager.h"
#include "Loopie/Core/Window.h"

#include <vector>
#include <nfd.h>

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
		static Application* s_Instance;

		std::vector<Module*> m_modules;
		ImGuiManager m_imguiManager;
		Window* m_window = nullptr;
		bool m_running = true;
	};

	//// Define Main
	/*int main() {
		Application* app = new "ApplicationType()";
		app->Run();
		delete app;
	}*/
}