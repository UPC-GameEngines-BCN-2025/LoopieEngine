#include "Window.h"

#include "Loopie/Core/Assert.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Core/Time.h"
#include "Loopie/Render/Renderer.h"
#include "Loopie/Core/InputEventManager.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>

namespace Loopie {
	Window::Window()
	{
		ASSERT(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS), 
	        "SDL_VIDEO could not initialize! SDL_Error: {0}", SDL_GetError());

    		// Set GL attributes BEFORE creating window
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		int width = WINDOW_DEFAULT_WIDTH;
		int height = WINDOW_DEFAULT_HEIGHT;

		const SDL_DisplayMode* display = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
		if (display) 
		{
	        	if (display->w < WINDOW_DEFAULT_WIDTH || display->h < WINDOW_DEFAULT_HEIGHT) 
			{
        	    		width = WINDOW_SMALL_DEFAULT_WIDTH;
	            		height = WINDOW_SMALL_DEFAULT_HEIGHT;
			}
	    	}

		m_window = SDL_CreateWindow("test_window_name", width, height,
	        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
    		ASSERT(m_window == nullptr, "Failed to create window! SDL_Error: {0}", SDL_GetError());

		// Create OpenGL context
		m_glContext = SDL_GL_CreateContext(m_window);
		ASSERT(m_glContext == NULL, "OpenGL context is NULL! SDL_Error: {0}", SDL_GetError());

		// Load OpenGL functions via GLAD
		// Renderer::Init(SDL_GL_GetProcAddress); /// Replace
		Renderer::Init((Renderer::GLLoader)SDL_GL_GetProcAddress);
		
		// Set clear color, optional
		Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });

		SetVsync(false, true);	
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

		Renderer::Shutdown();
	}

	// 24/09 Technically more of a swapbuffer rather than an update right now
	void Window::Update()
	{
		SDL_GL_SwapWindow(m_window);
		LimitFramerate();
	}

	void Window::ProcessEvents(InputEventManager& eventController) {
		if (eventController.HasEvent(SDL_EVENT_WINDOW_RESIZED)) {
			ivec2 windowSize = GetSize();
			Renderer::SetViewport(0, 0, windowSize.x, windowSize.y);
		}
	}

	ivec2 Window::GetSize() const /// Change To vector when posible (glm)
	{
		int x;
		int y;

		SDL_GetWindowSize(m_window, &x, &y);

		return { x, y };
	}

	ivec2 Window::GetPosition() const /// Change To vector when posible (glm)
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
			m_windowPrevWidth = GetSize().x;
			m_windowPrevHeight = GetSize().y;
		}
		ASSERT(!SDL_SetWindowSize(m_window, width, height), "Window couldn't change to new size!");
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
				ASSERT(!SDL_SetWindowFullscreenMode(m_window, mode), "Error setting fullscreen mode! SDL Error: {0}", SDL_GetError());
			}
			else
			{
				
			}
		}
		else 
		{
			
		}

		ASSERT(!SDL_SetWindowFullscreen(m_window, enable), "Error setting fullscreen! SDL Error: {0}", SDL_GetError());
	}

	void Window::SetResizable(bool enable)
	{
		SDL_SetWindowResizable(m_window, enable); // returns bool
	}

	void Window::SetVsync(bool enable, bool forceCall)
	{
		if (!forceCall && enable == m_vsyncState)
			return;
		ASSERT(!SDL_GL_SetSwapInterval(enable), "Error setting vsync! SDL Error: {0}", SDL_GetError());
		m_vsyncState = enable;
	}

	void Window::SetFramerateLimit(int fps)
	{
		m_targetFramerate = fps;
	}

	void Window::SetTitle(const char* title)
	{
		SDL_SetWindowTitle(m_window, title); // returns bool
	}

	void Window::SetPosition(int x, int y) 
	{
		SDL_SetWindowPosition(m_window, x, y); // returns bool
	}

	void Window::LimitFramerate()
	{
		if (m_vsyncState || m_targetFramerate <= 0)
			return;
		Uint64 now = SDL_GetPerformanceCounter();

		double freq = (double)(SDL_GetPerformanceFrequency());
		double frameTimeMs = (now - Time::GetLastFrameTime()) * 1000.0 / freq;
		double targetFrameMs = 1000.0 / m_targetFramerate;

		if (frameTimeMs < targetFrameMs)
		{
			Uint32 delayMs = (Uint32)(targetFrameMs - frameTimeMs);
			SDL_Delay(delayMs);
		}
	}
}

