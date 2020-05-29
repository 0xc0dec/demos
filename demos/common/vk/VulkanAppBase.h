/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../AppBase.h"
#include "VulkanDevice.h"

namespace vk
{
	class AppBase: public ::AppBase
	{
	public:
		AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

	protected:
		// TODO Better
		auto device() const -> vk::Device* { return dynamic_cast<vk::Device*>(device_.get()); }
	};
}
