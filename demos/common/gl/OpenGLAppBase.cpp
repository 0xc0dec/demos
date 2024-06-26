/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "OpenGLAppBase.h"
#include "OpenGLWindow.h"

gl::AppBase::AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen) : ::AppBase(std::make_unique<gl::Window>(canvasWidth, canvasHeight, "Demo", fullScreen))
{
}
