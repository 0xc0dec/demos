#include "ExampleBase.h"
#include <GL/glew.h>
#include <fstream>
#include <unordered_map>
#include <string>


ExampleBase::ExampleBase(int canvasWidth, int canvasHeight, bool fullScreen):
    canvasWidth(canvasWidth),
    canvasHeight(canvasHeight)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
        DIE("Failed to initialize SDL");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    auto x = SDL_WINDOWPOS_CENTERED;
    auto y = SDL_WINDOWPOS_CENTERED;
    auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullScreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    window = SDL_CreateWindow("Example 1", x, y, canvasWidth, canvasHeight, flags);
    context = SDL_GL_CreateContext(window);

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(1);
}


ExampleBase::~ExampleBase()
{
    if (context)
        SDL_GL_DeleteContext(context);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}


void ExampleBase::run()
{
    init();

    float lastTime = 0;
    auto run = true;
    while (run)
    {
        SDL_Event evt;
        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
                run = false;
                break;
            case SDL_KEYUP:
                if (evt.key.keysym.sym == SDLK_ESCAPE)
                    run = false;
                break;
            case SDL_WINDOWEVENT:
                if (evt.window.event == SDL_WINDOWEVENT_CLOSE)
                    run = false;
                break;
            }
        }

        auto time = SDL_GetTicks() / 1000.0f;
        auto dt = time - lastTime;
        lastTime = time;

        render(dt);

        SDL_GL_SwapWindow(window);
    }
}


GLuint ExampleBase::createProgram(const char* vs, const char* fs)
{
    auto vertex = compileShader(GL_VERTEX_SHADER, vs);
    auto fragment = compileShader(GL_FRAGMENT_SHADER, fs);
    auto program = linkProgram(vertex, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return program;
}


std::vector<uint8_t> ExampleBase::readFile(const char* path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        DIE("Failed to open file " << path);
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    auto bytes = std::vector<uint8_t>(size);
    file.read(reinterpret_cast<char*>(&bytes[0]), size);
    file.close();
    return bytes;
}


auto ExampleBase::compileShader(GLenum type, const char* src) -> GLuint
{
    static std::unordered_map<GLuint, std::string> typeNames =
    {
        { GL_VERTEX_SHADER, "vertex" },
        { GL_FRAGMENT_SHADER, "fragment" }
    };

    auto shader = glCreateShader(type);

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        glDeleteShader(shader);
        DIE("Failed to compile " << typeNames[type] << " shader: " << log.data());
    }

    return shader;
}


auto ExampleBase::linkProgram(GLuint vs, GLuint fs) -> GLint
{
    auto program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        glDeleteProgram(program);
        DIE("Failed to link program: " << log.data());
    }

    return program;
}