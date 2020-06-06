/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#pragma once

#include "../Common.h"

#ifdef WINDOWS_APP
#   define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan.h>

namespace vk
{
	constexpr void assertResult(const VkResult &result)
	{
		panicIf(result != VK_SUCCESS, "Vulkan API call failed");
	}
}
