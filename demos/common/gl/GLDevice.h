/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../Device.h"

class GLDevice final: public Device
{
public:
	GLDevice(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen);
	~GLDevice();

	auto sdlGLContext() const -> SDL_GLContext { return context_; }

	void endUpdate() override;
	
private:
    SDL_GLContext context_ = nullptr;
};
