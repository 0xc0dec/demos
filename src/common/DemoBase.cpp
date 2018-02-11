#include "DemoBase.h"
#include <GL/glew.h>
#include <fstream>
#include <unordered_map>
#include <string>

DemoBase::DemoBase(int canvasWidth, int canvasHeight, bool fullScreen):
    canvasWidth(canvasWidth),
    canvasHeight(canvasHeight)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
        DIE("Failed to initialize SDL");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    const auto x = SDL_WINDOWPOS_CENTERED;
    const auto y = SDL_WINDOWPOS_CENTERED;
    auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullScreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    window = SDL_CreateWindow("Demo", x, y, canvasWidth, canvasHeight, flags);
    context = SDL_GL_CreateContext(window);

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(1);
}

DemoBase::~DemoBase()
{
    if (context)
        SDL_GL_DeleteContext(context);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

void DemoBase::run()
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
                default:
                    break;
            }
        }

        const auto time = SDL_GetTicks() / 1000.0f;
        const auto dt = time - lastTime;
        lastTime = time;

        render(dt);

        SDL_GL_SwapWindow(window);
    }
}

auto DemoBase::createProgram(const char* vs, const char* fs) -> GLuint
{
    const auto vertex = compileShader(GL_VERTEX_SHADER, vs);
    const auto fragment = compileShader(GL_FRAGMENT_SHADER, fs);
    const auto program = linkProgram(vertex, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return program;
}

auto DemoBase::readFile(const char* path) -> std::vector<uint8_t>
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        DIE("Failed to open file " << path);
    
	const auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    auto bytes = std::vector<uint8_t>(size);
    file.read(reinterpret_cast<char*>(&bytes[0]), size);
    file.close();
    
	return bytes;
}

auto DemoBase::compileShader(GLenum type, const char* src) -> GLuint
{
    static std::unordered_map<GLuint, std::string> typeNames =
    {
        { GL_VERTEX_SHADER, "vertex" },
        { GL_FRAGMENT_SHADER, "fragment" }
    };

    const auto shader = glCreateShader(type);

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

auto DemoBase::linkProgram(GLuint vs, GLuint fs) -> GLint
{
    const auto program = glCreateProgram();
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