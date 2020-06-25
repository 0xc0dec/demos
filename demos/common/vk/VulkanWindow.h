/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../Window.h"
#include "VulkanResource.h"

namespace vk
{
	class Window final: public ::Window
	{
	public:
		Window(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen);

		void endUpdate() override;

        auto instance() const -> VkInstance { return instance_;  }
        auto surface() const -> VkSurfaceKHR { return surface_;  }

	private:
		vk::Resource<VkInstance> instance_;
		vk::Resource<VkSurfaceKHR> surface_;
	};
}
