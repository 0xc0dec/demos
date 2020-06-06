/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../AppBase.h"
#include "VulkanWindow.h"

namespace vk
{
	class AppBase: public ::AppBase
	{
	public:
		AppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

	protected:
		// TODO Better
		auto window() const -> vk::Window* { return dynamic_cast<vk::Window*>(window_.get()); }
	};
}
