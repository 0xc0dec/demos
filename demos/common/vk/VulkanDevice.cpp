/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VulkanDevice.h"
#include "VulkanCommon.h"
#include "../Common.h"
#include <SDL_syswm.h>

vk::Device::Device(uint32_t canvasWidth, uint32_t canvasHeight, const char *title, bool fullScreen):
	::Device(canvasWidth, canvasHeight)
{
	uint32_t flags = SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullScreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvasWidth, canvasHeight, flags);
	panicIf(!window_, "Unable to create device window");

	VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "";
    appInfo.pEngineName = "";
    appInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> enabledExtensions {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef WINDOWS_APP
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    };

    std::vector<const char*> enabledLayers {
        "VK_LAYER_LUNARG_standard_validation",
    };

	VkInstanceCreateInfo instanceInfo {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.pApplicationInfo = &appInfo;

	if (!enabledLayers.empty())
    {
        instanceInfo.enabledLayerCount = enabledLayers.size();
        instanceInfo.ppEnabledLayerNames = enabledLayers.data();
    }

    if (!enabledExtensions.empty())
    {
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
        instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();
    }

	instance_ = vk::Resource<VkInstance>{vkDestroyInstance};
    assertVkResult(vkCreateInstance(&instanceInfo, nullptr, instance_.cleanRef()));

#ifdef WINDOWS_APP
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);

    const auto hwnd = wmInfo.info.win.window;
    const auto hinstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

    VkWin32SurfaceCreateInfoKHR surfaceInfo;
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.flags = 0;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.hinstance = hinstance;
    surfaceInfo.hwnd = hwnd;

    surface_ = vk::Resource<VkSurfaceKHR>{instance_, vkDestroySurfaceKHR};
    assertVkResult(vkCreateWin32SurfaceKHR(instance_, &surfaceInfo, nullptr, surface_.cleanRef()));
#endif
}

void vk::Device::endUpdate()
{
}
