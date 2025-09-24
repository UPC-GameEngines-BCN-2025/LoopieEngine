#include "Window.h"
#include "SDL3/SDL_init.h"
#include "glad/glad.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Core/Log.h"


namespace Loopie {
	Window::Window()
	{
		// PSTODO: Ask if we will get the SDL error message like this
		// PSTODO: Verify if ASSERT works like this
		ASSERT(SDL_Init(SDL_INIT_VIDEO) < 0, "SDL_VIDEO could not initialize! SDL_Error: {0}", SDL_GetError());

		m_window = SDL_CreateWindow("test_window_name", WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE /*0*/); // Flags

		// Create OpenGL context
		m_glContext = SDL_GL_CreateContext(m_window);
		ASSERT(m_glContext == NULL, "OpenGL context is NULL!");

		// Load OpenGL functions via GLAD
		ASSERT(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) < 0, "Failed to Initialize GLAD!");
		
		// Set clear color, optional
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	}

	Window::~Window()
	{
		if (m_glContext != NULL)
		{
			SDL_GL_DestroyContext(m_glContext);
			m_glContext = NULL;
		}
		if (m_window != nullptr)
		{
			SDL_DestroyWindow(m_window);
			m_window = nullptr;
		}
	}

	// 24/09 Technically more of a swapbuffer rather than an update right now
	bool Window::Update()
	{
		bool returnStatus = true;

		SDL_GL_SwapWindow(m_window);

		return returnStatus;
	}

	std::pair<int, int> Window::GetSize() const
	{
		int x;
		int y;

		SDL_GetWindowSize(m_window, &x, &y);

		return { x, y };
	}

	std::pair<int, int> Window::GetPosition() const
	{
		int x;
		int y;

		SDL_GetWindowPosition(m_window, &x, &y);

		return { x, y };
	}

	bool Window::HasFocus() const
	{
		// Keyboard / input focus
		SDL_WindowFlags flags = SDL_GetWindowFlags(m_window);
		return (flags & SDL_WINDOW_INPUT_FOCUS) || (flags & SDL_WINDOW_MOUSE_FOCUS); 
	}

	bool Window::IsFullscreen() const
	{
		SDL_WindowFlags flags = SDL_GetWindowFlags(m_window);
		return (flags & SDL_WINDOW_FULLSCREEN);
	}

	void Window::SetWindowSize(int width, int height, bool savePreviousWindowsSize)
	{
		if (savePreviousWindowsSize)
		{
			m_windowPrevWidth = GetSize().first;
			m_windowPrevHeight = GetSize().second;
		}

		Log::Info("Attempting to change window width and height to {0}x{1}", width, height);

		ASSERT(SDL_SetWindowSize(m_window, width, height) < 0, "Window couldn't change to new size!");

		Log::Info("Changed window size succesfully!");
	}


	// https://wiki.libsdl.org/SDL3/SDL_SetWindowFullscreenMode
	// PSTODO: Check how to best store DisplayMode information
	// if mode is not nullptr, then fullscreen is window mode, otherwise it is borderless fullscreen
	void Window::SetWindowFullscreen(bool enable, const SDL_DisplayMode* mode)
	{

		if (enable) 
		{
			if (mode)
			{
				Log::Info("Enabling exclusive fullscreen: {0}x{1} @{2}Hz", mode->w, mode->h, mode->refresh_rate);
				ASSERT(SDL_SetWindowFullscreenMode(m_window, mode) < 0, "Error setting fullscreen mode! SDL Error: {0}", SDL_GetError());
			}
			else
			{
				Log::Info("Enabling borderless fullscreen.");
			}
		}
		else 
		{
			Log::Info("Disabling fullscreen.");
		}

		ASSERT(SDL_SetWindowFullscreen(m_window, enable) < 0, "Error setting fullscreen! SDL Error: {0}", SDL_GetError());
	}

	void Window::SetResizable(bool enable)
	{
		if (enable) Log::Info("Enabling resizable window.");
		else		Log::Info("Disabling resizable window.");

		SDL_SetWindowResizable(m_window, enable); // returns bool
	}

	void Window::SetTitle(const char* title)
	{
		Log::Info("Changing window title to {0}.", title);

		SDL_SetWindowTitle(m_window, title); // returns bool
	}

	void Window::SetPosition(int x, int y)
	{
		Log::Info("Changing window position to {0}x and {1}y.", x, y);

		SDL_SetWindowPosition(m_window, x, y); // returns bool
	}

}

