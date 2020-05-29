/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "../AppBase.h"
#include "VkDevice.h"

class VkAppBase: public AppBase
{
public:
	VkAppBase(uint32_t canvasWidth, uint32_t canvasHeight, bool fullScreen);

protected:
	// TODO Better
	auto device() const -> VkDevice* { return dynamic_cast<VkDevice*>(device_.get()); }
};
