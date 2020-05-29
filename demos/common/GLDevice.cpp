/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "GLDevice.h"
#include "Common.h"
#include <GL/glew.h>

GLDevice::GLDevice(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen):
	Device(canvasWidth, canvasHeight, title, fullScreen)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
        PANIC("Failed to initialize SDL");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    const auto x = SDL_WINDOWPOS_CENTERED;
    const auto y = SDL_WINDOWPOS_CENTERED;
    auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullScreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    window_ = SDL_CreateWindow(title, x, y, canvasWidth, canvasHeight, flags); // TODO create in base class
    context_ = SDL_GL_CreateContext(window_);

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(1);
}

GLDevice::~GLDevice()
{
	if (context_)
        SDL_GL_DeleteContext(context_);
}

void GLDevice::endUpdate()
{
	SDL_GL_SwapWindow(window_);
}
