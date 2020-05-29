/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "AppBase.h"
#include "GLDevice.h"

class GLAppBase: public AppBase
{
public:
	GLAppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

protected:
	// TODO Better
	auto device() const -> GLDevice* { return dynamic_cast<GLDevice*>(device_.get()); }
};
