/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "OpenGLAppBase.h"
#include "OpenGLWindow.h"

gl::AppBase::AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen)
{
	window_ = std::make_unique<gl::Window>(canvasWidth, canvasHeight, "Demo", fullScreen);
}
