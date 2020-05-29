/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../Device.h"

class VkDevice final: public Device
{
public:
	VkDevice(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen);
	~VkDevice() = default;

	auto sdlGLContext() const -> SDL_GLContext { return context_; }

	void endUpdate() override;
	
private:
    SDL_GLContext context_ = nullptr;
};
