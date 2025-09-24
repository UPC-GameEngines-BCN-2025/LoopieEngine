#pragma once
#include <SDL3/SDL_events.h>

namespace Loopie {
	class ImGuiManager {
	public:
		ImGuiManager() = default;
		~ImGuiManager();

		void Init();
		void GetEvents(const SDL_Event& event);
		void StartFrame();
		void EndFrame();
	};
}