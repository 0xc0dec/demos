/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "GLAppBase.h"
#include "GLDevice.h"

GLAppBase::GLAppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen)
{
	device_ = std::make_unique<GLDevice>(canvasWidth, canvasHeight, "Demo", fullScreen);
}
