#pragma once

namespace Loopie {
	class ImGuiManager {
	public:
		ImGuiManager();
		~ImGuiManager();

		void Init();
		void StartFrame();
		void EndFrame();
	};
}