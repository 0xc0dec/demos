/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../Device.h"
#include "VulkanCommon.h"

namespace vk
{
	class Device final: public ::Device
	{
	public:
		Device(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen);
		~Device() = default;

		void endUpdate() override;

	private:
	};
}
