
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL_opengles2.h>
#else
#include <glad/glad.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "events.h"
#include <iostream>
#include <fstream> 

// Texture
const char* cTextureFilename = "hellofix.png";
GLuint textureObj = 0;

// Vertex shader
GLint shaderPan, shaderZoom, shaderAspect;

const GLchar* vertexSource =
    "uniform vec2 pan;                                   \n"
    "uniform float zoom;                                 \n"
    "uniform float aspect;                               \n"
    "attribute vec4 position;                            \n"
    "varying vec2 texCoord;                              \n"
    "void main()                                         \n"
    "{                                                   \n"
    "    gl_Position = vec4(position.xyz, 1.0);          \n"
    "    gl_Position.xy += pan;                          \n"
    "    gl_Position.xy *= zoom;                         \n"
    "    texCoord = vec2(gl_Position.x, -gl_Position.y); \n"
    "    gl_Position.y *= aspect;                        \n"
    "}                                                   \n";

// Fragment/pixel shader
const GLchar* fragmentSource =
    "precision mediump float;                            \n"
    "varying vec2 texCoord;                              \n"
    "uniform sampler2D texSampler;                       \n"
    "void main()                                         \n"
    "{                                                   \n"
    "    gl_FragColor = texture2D(texSampler, texCoord); \n"
    "}                                                   \n";

void updateShader(EventHandler& eventHandler)
{
    Camera& camera = eventHandler.camera();

    glUniform2fv(shaderPan, 1, camera.pan());
    glUniform1f(shaderZoom, camera.zoom()); 
    glUniform1f(shaderAspect, camera.aspect());
}

GLuint initShader(EventHandler& eventHandler)
{
    // Create and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Link vertex and fragment shader into shader program and use it
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Get shader variables and initalize them
    shaderPan = glGetUniformLocation(shaderProgram, "pan");
    shaderZoom = glGetUniformLocation(shaderProgram, "zoom");    
    shaderAspect = glGetUniformLocation(shaderProgram, "aspect");
    updateShader(eventHandler);

    return shaderProgram;
}

void initGeometry(GLuint shaderProgram)
{
    // Create vertex buffer object and copy vertex data into it
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLfloat triangleVertices[] = 
    {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    
    // Specify the layout of the shader vertex data (positions only, 3 floats)
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void initTexture()
{
    SDL_Surface *image = IMG_Load(cTextureFilename);

    if (!image)
    {
        // Create a fallback gray texture
        printf("Failed to load %s, due to %s\n", cTextureFilename, IMG_GetError());
        const int w = 128, h = 128, bitsPerPixel = 24;
        image = SDL_CreateRGBSurface(0, w, h, bitsPerPixel, 0, 0, 0, 0);
        if (image)
            memset(image->pixels, 0x42, image->w * image->h * bitsPerPixel / 8);
    }

    if (image)
    {
        int bitsPerPixel = image->format->BitsPerPixel;
        printf ("Image dimensions %dx%d, %d bits per pixel\n", image->w, image->h, bitsPerPixel);

        // Determine GL texture format
        GLint format = -1;
        if (bitsPerPixel == 24)
            format = GL_RGB;
        else if (bitsPerPixel == 32)
            format = GL_RGBA;

        if (format != -1)
        {
            // Generate a GL texture object
            glGenTextures(1, &textureObj);

            // Bind GL texture
            glBindTexture(GL_TEXTURE_2D, textureObj);

            // Set the GL texture's wrapping and stretching properties
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Copy SDL surface image to GL texture
            glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0,
                         format, GL_UNSIGNED_BYTE, image->pixels);
        }
                                 
        SDL_FreeSurface (image);        
    }                       
}

void redraw(EventHandler& eventHandler)
{
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the vertex buffer
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Swap front/back framebuffers
    eventHandler.swapWindow();
}

void mainLoop(void* mainLoopArg) 
{    
    EventHandler& eventHandler = *((EventHandler*)mainLoopArg);
    eventHandler.processEvents();

    // Update shader if camera changed
    if (eventHandler.camera().updated())
        updateShader(eventHandler);

    redraw(eventHandler);
}

int main(int argc, char** argv)
{
    std::ifstream input("read.txt");
    std::string line;
    while (std::getline(input, line))
    {
	    std::cout << line;
    }
    
    //std::ostream output{};


    EventHandler eventHandler("Hello Texture");

    #ifndef __EMSCRIPTEN__
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    #endif

    // Initialize shader, geometry, and texture
    GLuint shaderProgram = initShader(eventHandler);
    initGeometry(shaderProgram);
    initTexture();

    // Start the main loop
    void* mainLoopArg = &eventHandler;

#ifdef __EMSCRIPTEN__
    int fps = 0; // Use browser's requestAnimationFrame
    emscripten_set_main_loop_arg(mainLoop, mainLoopArg, fps, true);
#else
    while(true) 
        mainLoop(mainLoopArg);
#endif

    glDeleteTextures(1, &textureObj);

    return 0;
}