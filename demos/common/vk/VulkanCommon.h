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

#define ASSERT_VK_RESULT(vkCall) panicIf((vkCall) != VK_SUCCESS, "Vulkan API call failed")