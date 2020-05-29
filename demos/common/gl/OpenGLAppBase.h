/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../AppBase.h"
#include "OpenGLDevice.h"

namespace gl
{
	class AppBase: public ::AppBase
	{
	public:
		AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

	protected:
		// TODO Better
		auto device() const -> gl::Device* { return dynamic_cast<gl::Device*>(device_.get()); }
	};
}
