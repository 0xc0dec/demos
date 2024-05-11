/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include "../AppBase.h"
#include "VulkanWindow.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

namespace vk
{
    class AppBase : public ::AppBase
    {
    public:
        AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

    protected:
        // TODO avoid casting
        auto window() const -> vk::Window * { return dynamic_cast<Window *>(window_.get()); }
        auto swapchain() -> Swapchain & { return swapchain_; }
        auto device() -> Device & { return device_; }

    private:
        Device device_;
        Swapchain swapchain_;
    };
}
