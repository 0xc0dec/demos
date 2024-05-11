/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "OpenGLWindow.h"
#include "../Common.h"
#include <GL/glew.h>

gl::Window::Window(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen) : ::Window(canvasWidth, canvasHeight)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullScreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    // TODO create in base class
    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvasWidth, canvasHeight, flags);

    context_ = SDL_GL_CreateContext(window_);

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(1);
}

gl::Window::~Window()
{
    if (context_)
        SDL_GL_DeleteContext(context_);
}

void gl::Window::endUpdate()
{
    SDL_GL_SwapWindow(window_);
}
