/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include "../Common.h"
#include "VulkanResource.h"

#ifdef WINDOWS_APP
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan.h>
#include <vector>

namespace vk
{
    auto createSemaphore(VkDevice device) -> vk::Resource<VkSemaphore>;
    void queueSubmit(VkQueue queue, uint32_t waitSemaphoreCount, const VkSemaphore *waitSemaphores,
                     uint32_t signalSemaphoreCount, const VkSemaphore *signalSemaphores,
                     uint32_t commandBufferCount, const VkCommandBuffer *commandBuffers);
    auto findMemoryType(VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, uint32_t typeBits,
                        VkMemoryPropertyFlags properties) -> int;
    auto createFrameBuffer(VkDevice device, const std::vector<VkImageView> &attachments,
                           VkRenderPass renderPass, uint32_t width, uint32_t height) -> vk::Resource<VkFramebuffer>;
    auto createImageView(VkDevice device, VkFormat format, VkImageViewType type, uint32_t mipLevels, uint32_t layers,
                         VkImage image, VkImageAspectFlags aspectMask) -> vk::Resource<VkImageView>;
    auto makeImagePipelineBarrier(VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                                  VkImageSubresourceRange subresourceRange) -> VkImageMemoryBarrier;

    constexpr void ensure(const VkResult &result)
    {
        panicIf(result != VK_SUCCESS, "Vulkan API call failed");
    }
}
