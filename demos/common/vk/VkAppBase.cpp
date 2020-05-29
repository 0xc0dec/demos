/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VkAppBase.h"
#include "VkDevice.h"

VkAppBase::VkAppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen)
{
	device_ = std::make_unique<VkDevice>(canvasWidth, canvasHeight, "Demo", fullScreen);
}
