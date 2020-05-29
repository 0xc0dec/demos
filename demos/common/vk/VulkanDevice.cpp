/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VulkanDevice.h"
#include "../Common.h"
#include <SDL_syswm.h>

vk::Device::Device(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen):
	::Device(canvasWidth, canvasHeight)
{
	uint32_t flags = SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullScreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvasWidth, canvasHeight, flags);
	if (!window_)
		PANIC("Unable to create device window");

	VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "";
    appInfo.pEngineName = "";
    appInfo.apiVersion = VK_API_VERSION_1_0;
}

void vk::Device::endUpdate()
{
}
