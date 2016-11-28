#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <vector>

#define DIE(msg) do { std::cerr << msg << std::endl; exit(1); } while (0)


class ExampleBase
{
public:
    ExampleBase(int canvasWidth, int canvasHeight, bool fullScreen);
    virtual ~ExampleBase();

    void run();

protected:
    static auto createProgram(const char* vs, const char* fs) -> GLuint;
    static auto readFile(const char* path) -> std::vector<uint8_t>;

    virtual void init() = 0;
    virtual void shutdown() = 0;
    virtual void render(float dt) = 0;

    uint32_t canvasWidth = 0;
    uint32_t canvasHeight = 0;

private:
    static auto compileShader(GLenum type, const char* src) -> GLuint;
    static auto linkProgram(GLuint vs, GLuint fs) -> GLint;

    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
};