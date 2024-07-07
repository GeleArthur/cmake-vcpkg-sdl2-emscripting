#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL_opengles2.h>
#else
#include <glad/glad.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>

SDL_Window* m_pWindow;
SDL_GLContext m_pContext;
void checkCompileErrors(unsigned int shader, std::string type);

GLint CreateShader(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    // open files
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    vertexCode   = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexCodePtr = vertexCode.c_str();
    glShaderSource(vertex, 1, &vertexCodePtr, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentCodePtr = fragmentCode.c_str();
    glShaderSource(fragment, 1, &fragmentCodePtr, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    GLuint ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

//TODO: Change
void checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << '\n' << infoLog << '\n';
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << '\n' << infoLog << '\n';
        }
    }
}

GLuint LoadTexture()
{
    SDL_Surface *image = IMG_Load("Resources/hello.png");

    if(!image){
        throw;
    }
    int bitsPerPixel = image->format->BitsPerPixel;

    GLint format = -1;
        if (bitsPerPixel == 24)
            format = GL_RGB;
        else if (bitsPerPixel == 32)
            format = GL_RGBA;

    GLuint textureObj = 0;
    glGenTextures(1, &textureObj);

    glBindTexture(GL_TEXTURE_2D, textureObj);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);

    return textureObj;
}

void mainLoop(void* input)
{
    SDL_Event e{};

    while (SDL_PollEvent(&e) != 0)
	{
		// Handle the polled event
		switch (e.type)
		{
		case SDL_QUIT:
            std::exit(0);
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

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the vertex buffer
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(m_pWindow);
};

int main(int argc, char** argv)
{
    m_pWindow = 
        SDL_CreateWindow("CMake SDL2 VCPKG emscripten OpenGlES example", 
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            640, 640, 
                            SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    SDL_GL_SetSwapInterval(1);

    m_pContext = SDL_GL_CreateContext(m_pWindow);
    
    #ifndef __EMSCRIPTEN__
    gladLoadGLES2Loader(SDL_GL_GetProcAddress); // 100% no documentation for this
    #endif

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 640, 640);

    GLuint shaderProgram = CreateShader("Resources/vertexShader.vs", "Resources/fragmentShader.fs");

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // We bind buffer here

    float positions[] = {
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.5, 0.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_DYNAMIC_DRAW);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "VertexPosition");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, false, 0, nullptr);
    glUseProgram(shaderProgram); // We bind Program here

    GLuint textureId = LoadTexture();



#ifdef __EMSCRIPTEN__
    int fps = 0; // Use browser's requestAnimationFrame
    emscripten_set_main_loop_arg(mainLoop, nullptr, fps, true);
#else
    while(true) {
        mainLoop(nullptr);
    }

#endif

    glDeleteTextures(1, &textureId);

    return 0;
}