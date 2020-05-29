/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "OpenGLAppBase.h"
#include "OpenGLDevice.h"

gl::AppBase::AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen)
{
	device_ = std::make_unique<gl::Device>(canvasWidth, canvasHeight, "Demo", fullScreen);
}
