#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <Loopie/Core/Assert.h>
#include <iostream>

using namespace Loopie;

int main() {
    Log::Init();

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        ASSERT(true, "Failed to initialize SDL: {0}", SDL_GetError());
        return 0;
    }
    Log::Info("SDL initialized");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* m_window = SDL_CreateWindow("Hello OpenGL",
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        ASSERT(true, "Failed to create SDL window: {}", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    Log::Info("Window created successfully");

    SDL_GLContext m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        ASSERT(true, "Failed to create OpenGL context: {}", SDL_GetError());
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 0;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        ASSERT(true, "Failed to initialize GLAD");
        SDL_GL_DestroyContext(m_glContext);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 0; 
    }
    Log::Info("GLAD initialized");


    bool close = false;
    while (!close) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                close = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    SDL_Event quitEvent;
                    quitEvent.type = SDL_EVENT_QUIT;
                    SDL_PushEvent(&quitEvent);
                }
            }
            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                glViewport(0, 0, event.window.data1, event.window.data2);
            }
        }

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(m_window);
    }

    Log::Info("Shutting down...");
    if (m_glContext) {
        SDL_GL_DestroyContext(m_glContext);
        m_glContext = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();

    return 0;
}