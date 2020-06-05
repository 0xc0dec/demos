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

constexpr void assertVkResult(const VkResult &result)
{
	panicIf(result != VK_SUCCESS, "Vulkan API call failed");
}
