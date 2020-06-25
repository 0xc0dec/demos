/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../Window.h"

namespace gl
{
	class Window final: public ::Window
	{
	public:
		Window(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen);
		~Window();

		auto sdlGLContext() const -> SDL_GLContext { return context_; }

		void endUpdate() override;

	private:
		SDL_GLContext context_ = nullptr;
	};
}
