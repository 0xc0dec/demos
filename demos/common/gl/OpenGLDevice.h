/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../Device.h"

namespace gl
{
	class Device final: public ::Device
	{
	public:
		Device(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen);
		~Device();

		auto sdlGLContext() const -> SDL_GLContext { return context_; }

		void endUpdate() override;

	private:
		SDL_GLContext context_ = nullptr;
	};
}
