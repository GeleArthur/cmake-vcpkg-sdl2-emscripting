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
#include <filesystem>
#include <chrono>

SDL_Window* m_pWindow;
SDL_GLContext m_pContext;

float moveX{0};
float moveY{0};

GLint matrixLocation{-1};
std::chrono::steady_clock::time_point t1;

void CheckShaderErrors(unsigned int shader)
{
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR" << '\n' << infoLog << '\n';
    }

}

void CheckProgramErrors(unsigned int program)
{
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR" << '\n' << infoLog << '\n';
    }
}

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
    CheckShaderErrors(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentCodePtr = fragmentCode.c_str();
    glShaderSource(fragment, 1, &fragmentCodePtr, nullptr);
    glCompileShader(fragment);
    CheckShaderErrors(fragment);

    GLuint ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    CheckProgramErrors(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

GLuint LoadTexture()
{
    SDL_Surface *image = IMG_Load("Resources/image.png");

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

void MainLoop(void* input)
{
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    float elapsedSeconds = std::chrono::duration<float>(t2 - t1).count();
    t1 = t2;

    SDL_Event e{};

    while (SDL_PollEvent(&e) != 0)
	{
		// Handle the polled event
		switch (e.type)
		{
		case SDL_QUIT:
            std::exit(0);
			break;
		default:
		    break;
		}
	}
    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
    if(keyboardState[SDL_SCANCODE_UP])
    {
        moveY -= 1;
    }
    if(keyboardState[SDL_SCANCODE_DOWN])
    {
        moveY += 1;
    }
    if(keyboardState[SDL_SCANCODE_RIGHT])
    {
        moveX += 1;
    }
    if(keyboardState[SDL_SCANCODE_LEFT])
    {
        moveX -= 1;
    }

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    float positions[] = {
        moveX , moveY ,
        320, 320,
        0, 320 ,
    };

    std::cout << moveX << ":" << moveY << std::endl;

    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_DYNAMIC_DRAW);

    float l = 0;
    float r = 640;
    float b = 640;
    float t = 0;
    float n = 1;
    float f = -1;

    GLfloat orthGraphicsMatrix[16]{
        2.0f/(r-l),0.0f,0.0f, -((r+l)/(r-l)),
        0,2.0f/(t-b),0,       -((t+b)/(t-b)),
        0,0, -2.0f/(f-n),      -((f+n)/(f-n)),
        0,0,0,                1
    };

    glUniformMatrix4fv(matrixLocation, 1, true, orthGraphicsMatrix);

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
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

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

    GLuint shaderProgram = CreateShader("Resources/vertexShader.vert", "Resources/fragmentShader.frag");
    glUseProgram(shaderProgram); // We bind Program here

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // We bind buffer here

    GLint posAttrib = glGetAttribLocation(shaderProgram, "VertexPosition");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, false, 0, nullptr);

    matrixLocation = glGetUniformLocation(shaderProgram, "u_matrix");
    
    // float l = -640;
    // float r = 640;
    // float t = 640;
    // float b = -640;
    // float f = -100;
    // float n = 100;

    // GLfloat orthGraphicsMatrix[16]{
    //     2.0f/(r-l),0.0f,0.0f, -((r+l)/(r-l)),
    //     0,2.0f/(t-b),0,-((t+b)/(t-b)),
    //     0,0,2.0f/(f-n), -((f+n)/(f-n)),
    //     0,0,0,1
    // };

    // GLfloat identityMatrix[16]{
    //     1,0,0, 0,
    //     0,1,0,0,
    //     0,0,1,0,
    //     0,0,0,1
    // };

    // glUniform1f(matrixLocation, 1);
    // glUniformMatrix4fv(matrixLocation, 1, false, orthGraphicsMatrix);

    GLuint textureId = LoadTexture();

    t1 = std::chrono::steady_clock::now();

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_BLEND);



#ifdef __EMSCRIPTEN__
    int fps = 0; // Use browser's requestAnimationFrame
    emscripten_set_main_loop_arg(MainLoop, nullptr, fps, true);
#else
    while(true) {
        MainLoop(nullptr);
    }

#endif

    glDeleteTextures(1, &textureId);

    return 0;
}