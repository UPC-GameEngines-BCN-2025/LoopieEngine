#pragma once

#include "Loopie/Core/Module.h"

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

	private:
		static Application* s_Instance;

		std::vector<Module*> m_Modules;
		bool m_running = true;
	};

	//// Define Main
	/*int main() {
		Application* app = new "ApplicationType()";
		app->Run();
		delete app;
	}*/
}