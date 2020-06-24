/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VulkanAppBase.h"
#include "VulkanWindow.h"

vk::AppBase::AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen)
{
	window_ = std::make_unique<vk::Window>(canvasWidth, canvasHeight, "Demo", fullScreen);

}
