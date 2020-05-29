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

#define CHECK_VK_RESULT(vkCall) if ((vkCall) != VK_SUCCESS) { PANIC("Vulkan API call failed"); }