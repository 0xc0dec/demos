/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "VulkanDevice.h"
#include "VulkanCommon.h"
#include <iostream>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunc(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
                                                        uint64_t obj, size_t location, int code, const char *layerPrefix, const char *msg, void *userData)
{
    std::cout << msg << std::endl;
    return VK_FALSE;
}

static auto createDebugCallback(VkInstance instance, PFN_vkDebugReportCallbackEXT callbackFunc) -> vk::Resource<VkDebugReportCallbackEXT>
{
    VkDebugReportCallbackCreateInfoEXT createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = callbackFunc;

    const auto create = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
    panicIf(!create, "Unable to load pointer to vkCreateDebugReportCallbackEXT");

    const auto destroy = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
    panicIf(!destroy, "Unable to load pointer to vkDestroyDebugReportCallbackEXT");

    vk::Resource<VkDebugReportCallbackEXT> result{instance, destroy};
    vk::ensure(create(instance, &createInfo, nullptr, result.cleanRef()));

    return result;
}

static auto selectSurfaceFormat(VkPhysicalDevice device, VkSurfaceKHR surface) -> std::tuple<VkFormat, VkColorSpaceKHR>
{
    uint32_t count;
    vk::ensure(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr));

    std::vector<VkSurfaceFormatKHR> formats(count);
    vk::ensure(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, formats.data()));

    if (count == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        return {VK_FORMAT_B8G8R8A8_UNORM, formats[0].colorSpace};
    return {formats[0].format, formats[0].colorSpace};
}

static auto selectQueueIndex(VkPhysicalDevice device, VkSurfaceKHR surface) -> uint32_t
{
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

    std::vector<VkQueueFamilyProperties> queueProps;
    queueProps.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queueProps.data());

    std::vector<VkBool32> presentSupported(count);
    for (uint32_t i = 0; i < count; i++)
        vk::ensure(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupported[i]));

    // TODO support for separate rendering and presenting queues
    for (uint32_t i = 0; i < count; i++)
    {
        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupported[i] == VK_TRUE)
            return i;
    }

    panic("Failed to find queue index");

    return 0;
}

static auto createDevice(VkPhysicalDevice physicalDevice, uint32_t queueIndex) -> vk::Resource<VkDevice>
{
    std::vector<float> queuePriorities = {0.0f};
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities.data();

    std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkPhysicalDeviceFeatures enabledFeatures{};
    enabledFeatures.samplerAnisotropy = true;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    vk::Resource<VkDevice> result{vkDestroyDevice};
    vk::ensure(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, result.cleanRef()));

    return result;
}

static auto createCommandPool(VkDevice device, uint32_t queueIndex) -> vk::Resource<VkCommandPool>
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vk::Resource<VkCommandPool> commandPool{device, vkDestroyCommandPool};
    vk::ensure(vkCreateCommandPool(device, &poolInfo, nullptr, commandPool.cleanRef()));

    return commandPool;
}

vk::Device::Device(VkInstance instance, VkSurfaceKHR surface) : surface_(surface)
{
#ifdef DEMOS_DEBUG
    debugCallback_ = createDebugCallback(instance, debugCallbackFunc);
#endif

    selectPhysicalDevice(instance);

    detectFormatSupport(VK_FORMAT_R8_UNORM);
    detectFormatSupport(VK_FORMAT_R8G8B8A8_UNORM);
    detectFormatSupport(VK_FORMAT_R16G16B16A16_SFLOAT);
    detectFormatSupport(VK_FORMAT_D32_SFLOAT);
    detectFormatSupport(VK_FORMAT_D32_SFLOAT_S8_UINT);
    detectFormatSupport(VK_FORMAT_D24_UNORM_S8_UINT);
    detectFormatSupport(VK_FORMAT_D16_UNORM_S8_UINT);
    detectFormatSupport(VK_FORMAT_D16_UNORM);

    auto surfaceFormats = ::selectSurfaceFormat(physical_, surface);
    colorFormat_ = std::get<0>(surfaceFormats);
    colorSpace_ = std::get<1>(surfaceFormats);
    depthFormat_ = selectDepthFormat();

    queueIndex_ = selectQueueIndex(physical_, surface);
    handle_ = createDevice(physical_, queueIndex_);
    vkGetDeviceQueue(handle_, queueIndex_, 0, &queue_);

    commandPool_ = createCommandPool(handle_, queueIndex_);
}

bool vk::Device::isFormatSupported(VkFormat format, VkFormatFeatureFlags features) const
{
    return supportedFormats_.count(format) && (supportedFormats_.at(format) & features) == features;
}

void vk::Device::selectPhysicalDevice(VkInstance instance)
{
    uint32_t gpuCount = 0;
    ensure(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));

    std::vector<VkPhysicalDevice> devices(gpuCount);
    ensure(vkEnumeratePhysicalDevices(instance, &gpuCount, devices.data()));

    for (const auto &device : devices)
    {
        vkGetPhysicalDeviceProperties(device, &physicalProperties_);
        vkGetPhysicalDeviceFeatures(device, &physicalFeatures_);
        vkGetPhysicalDeviceMemoryProperties(device, &physicalMemoryFeatures_);

        physical_ = device;

        // Select discrete GPU if present, otherwise any other
        if (physicalProperties_.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return;
    }
}

void vk::Device::detectFormatSupport(VkFormat format)
{
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(physical_, format, &formatProps);
    if (formatProps.optimalTilingFeatures)
        supportedFormats_[format] = formatProps.optimalTilingFeatures;
}

auto vk::Device::selectDepthFormat() const -> VkFormat
{
    std::vector<VkFormat> depthFormats =
        {
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM};

    for (auto &format : depthFormats)
    {
        if (isFormatSupported(format, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
            return format;
    }

    panic("Failed to pick depth format");

    return VK_FORMAT_UNDEFINED;
}
