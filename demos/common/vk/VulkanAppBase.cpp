/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VulkanAppBase.h"
#include "VulkanWindow.h"

vk::AppBase::AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen):
    ::AppBase(std::make_unique<vk::Window>(canvasWidth, canvasHeight, "Demo", fullScreen))
{
    device_ = vk::Device(window()->instance(), window()->surface());
    swapchain_ = Swapchain(device_, canvasWidth, canvasHeight, false); // TODO configure vsync
}
