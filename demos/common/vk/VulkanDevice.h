/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../Device.h"
#include "VulkanCommon.h"
#include "VulkanResource.h"

namespace vk
{
	class Device final: public ::Device
	{
	public:
		Device(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen);
		~Device();

		void endUpdate() override;

	private:
		vk::Resource<VkInstance> instance_;
		vk::Resource<VkSurfaceKHR> surface_;
	};
}
