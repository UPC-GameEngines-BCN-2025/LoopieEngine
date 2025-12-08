#pragma once

#include "Loopie/Math/MathTypes.h"

#include <string>
#include <SDL3/SDL_video.h>

#define WINDOW_DEFAULT_WIDTH 1600
#define WINDOW_DEFAULT_HEIGHT 900

#define WINDOW_SMALL_DEFAULT_WIDTH 1152
#define WINDOW_SMALL_DEFAULT_HEIGHT 648


namespace Loopie {
	class InputEventManager;

	class Window {
	public:
		Window();
		~Window();

		void ProcessEvents(InputEventManager& eventController);
		void Update();

		ivec2 GetSize() const;
		ivec2 GetPosition() const;
		bool HasFocus() const;
		bool IsFullscreen() const;

		// TODO? Set logic resolution? https://wiki.libsdl.org/SDL3/SDL_SetRenderLogicalPresentation
		void SetWindowSize(int width, int height, bool savePreviousWindowsSize = false);
		void SetWindowFullscreen(bool enable, const SDL_DisplayMode* mode = nullptr);
		void SetResizable(bool enable);/// Maybe Do a getter also?
		void SetVsync(bool enable, bool forceCall = false);
		void SetFramerateLimit(int fps);
		void SetTitle(const char* title);/// Maybe Do a getter also?
		void SetPosition(int x, int y);
		// missing borderless, vsync, multi-monitor?

		SDL_Window* GetSDLWindow() const { return m_window; }
		SDL_GLContext GetSDL_GL_Context() const { return m_glContext; }
		bool IsVsyncEnabled() const { return m_vsyncState; };
		int GetFramerateLimit() const { return m_targetFramerate; }

	private:
		void LimitFramerate();

	private:
		SDL_Window* m_window = nullptr;
		SDL_GLContext m_glContext = NULL;
		int m_windowPrevWidth = 0;
		int m_windowPrevHeight = 0;

		bool m_vsyncState = false;

		int m_targetFramerate = 60;
	};
}

