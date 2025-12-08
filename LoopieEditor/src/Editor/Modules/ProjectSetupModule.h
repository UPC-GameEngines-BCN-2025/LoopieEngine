#pragma once

#include "Loopie/Core/Module.h"
#include "Editor/Interfaces/ProjectSetup/ProjectSetupInterface.h"

namespace Loopie {
	class ProjectSetupModule : public Module{
	public:
		ProjectSetupModule() = default;
		~ProjectSetupModule() = default;

		void OnUpdate() override;
		void OnLoad()override;
		void OnUnload()override;
		void OnInterfaceRender()override;
	private:

	private:
		ProjectSetupInterface* m_interface = nullptr;
	};
}