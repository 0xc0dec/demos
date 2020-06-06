/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../AppBase.h"
#include "OpenGLWindow.h"

namespace gl
{
	class AppBase: public ::AppBase
	{
	public:
		AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

	protected:
		// TODO Better
		auto window() const -> gl::Window* { return dynamic_cast<gl::Window*>(window_.get()); }
	};
}
