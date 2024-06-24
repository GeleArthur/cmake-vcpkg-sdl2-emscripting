#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL_opengles2.h>
#else
#include <glad/glad.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <ctime>

SDL_Window* m_pWindow;
SDL_GLContext m_pContext;

void mainLoop()
{
    SDL_Event e{};

    while (SDL_PollEvent(&e) != 0)
	{
		// Handle the polled event
		switch (e.type)
		{
		case SDL_QUIT:
			break;
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEWHEEL:
            break;
		case SDL_MOUSEBUTTONDOWN:
			break;
		case SDL_MOUSEBUTTONUP:
			break;
		}
	}

    SDL_GL_SwapWindow(m_pWindow);
};

int main(int argc, char** argv)
{
    srand(static_cast<unsigned int>(time(nullptr)));

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    m_pWindow = 
        SDL_CreateWindow("CMake SDL2 VCPKG emscripten OpenGlES example", 
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            640, 640, 
                            SDL_WINDOW_OPENGL);

    m_pContext = SDL_GL_CreateContext(m_pWindow);

    SDL_GL_SetSwapInterval(1);
    glViewport(0, 0, 640, 640);

    #ifndef __EMSCRIPTEN__
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    #endif

    // Initialize shader, geometry, and texture
    // GLuint shaderProgram = initShader(eventHandler);
    // initGeometry(shaderProgram);
    // initTexture();

    // Start the main loop
    // void* mainLoopArg = &eventHandler;

#ifdef __EMSCRIPTEN__
    int fps = 0; // Use browser's requestAnimationFrame
    // emscripten_set_main_loop_arg(mainLoop, mainLoopArg, fps, true);
#else
    while(true) {
        mainLoop();
    }

#endif

    // glDeleteTextures(1, &textureObj);

    return 0;
}