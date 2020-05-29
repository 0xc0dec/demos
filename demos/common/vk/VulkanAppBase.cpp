/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VulkanAppBase.h"
#include "VulkanDevice.h"

vk::AppBase::AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen)
{
	device_ = std::make_unique<vk::Device>(canvasWidth, canvasHeight, "Demo", fullScreen);
}
