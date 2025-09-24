#pragma once

#include "SDL3/SDL_video.h"
#include <utility> // Added for std::pair

#define WINDOW_DEFAULT_WIDTH 800
#define WINDOW_DEFAULT_HEIGHT 600

struct SDL_Window;

namespace Loopie {
	class Window {
	public:
		Window();
		~Window();

		bool Update();

		std::pair<int, int> GetSize() const;
		std::pair<int, int> GetPosition() const;
		bool HasFocus() const;
		bool IsFullscreen() const;

		// TODO? Set logic resolution? https://wiki.libsdl.org/SDL3/SDL_SetRenderLogicalPresentation
		void SetWindowSize(int width, int height, bool savePreviousWindowsSize = false);
		void SetWindowFullscreen(bool enable, const SDL_DisplayMode* mode = nullptr);
		void SetResizable(bool enable);
		void SetTitle(const char* title);
		void SetPosition(int x, int y);
		// missing borderless, vsync, multi-monitor?

		// SDL_Window* GetSDLHandle() const { return m_Window; } // Necessary?

	private:
		SDL_Window* m_window = nullptr;
		SDL_GLContext m_glContext = NULL;
		int m_windowPrevWidth = 0;
		int m_windowPrevHeight = 0;
	};
}

