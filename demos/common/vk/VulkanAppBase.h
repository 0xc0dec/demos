/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../AppBase.h"
#include "VulkanWindow.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

namespace vk
{
	class AppBase: public ::AppBase
	{
	public:
		AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

	protected:
		// TODO avoid casting
		auto window() const -> vk::Window* { return dynamic_cast<Window*>(window_.get()); }

	private:
        Device device_;
        Swapchain swapchain_;
	};
}
