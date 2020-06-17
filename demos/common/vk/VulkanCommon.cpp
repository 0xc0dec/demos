/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#include "VulkanCommon.h"
#include <vector>

using namespace vk;

auto vk::createSemaphore(VkDevice device) -> Resource<VkSemaphore>
{
    VkSemaphoreCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    Resource<VkSemaphore> semaphore{device, vkDestroySemaphore};
    assertResult(vkCreateSemaphore(device, &info, nullptr, semaphore.cleanRef()));

    return semaphore;
}

void vk::queueSubmit(VkQueue queue, uint32_t waitSemaphoreCount, const VkSemaphore *waitSemaphores,
    uint32_t signalSemaphoreCount, const VkSemaphore *signalSemaphores, uint32_t commandBufferCount,
    const VkCommandBuffer *commandBuffers)
{
    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.pWaitDstStageMask = &submitPipelineStages;
    info.waitSemaphoreCount = waitSemaphoreCount;
    info.pWaitSemaphores = waitSemaphores;
    info.signalSemaphoreCount = signalSemaphoreCount;
    info.pSignalSemaphores = signalSemaphores;
    info.commandBufferCount = commandBufferCount;
    info.pCommandBuffers = commandBuffers;
    assertResult(vkQueueSubmit(queue, 1, &info, VK_NULL_HANDLE));
}

auto vk::findMemoryType(VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties, uint32_t typeBits,
    VkMemoryPropertyFlags properties) -> int
{
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((typeBits & 1) == 1)
        {
            if ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }
        typeBits >>= 1;
    }
    return -1;
}

auto vk::createFrameBuffer(VkDevice device, const std::vector<VkImageView> &attachments, VkRenderPass renderPass,
    uint32_t width, uint32_t height) -> vk::Resource<VkFramebuffer>
{
    VkFramebufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.renderPass = renderPass;
    info.attachmentCount = attachments.size();
    info.pAttachments = attachments.data();
    info.width = width;
    info.height = height;
    info.layers = 1;

    Resource<VkFramebuffer> frameBuffer{device, vkDestroyFramebuffer};
    assertResult(vkCreateFramebuffer(device, &info, nullptr, frameBuffer.cleanRef()));

    return frameBuffer;
}

auto vk::createImageView(VkDevice device, VkFormat format, VkImageViewType type, uint32_t mipLevels, uint32_t layers,
    VkImage image, VkImageAspectFlags aspectMask) -> vk::Resource<VkImageView>
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.viewType = type;
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.format = format;
    viewInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    viewInfo.subresourceRange = {};
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = layers;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.image = image;

    Resource<VkImageView> view{device, vkDestroyImageView};
    assertResult(vkCreateImageView(device, &viewInfo, nullptr, view.cleanRef()));

    return view;
}

auto vk::makeImagePipelineBarrier(VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
    VkImageSubresourceRange subresourceRange) -> VkImageMemoryBarrier
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.oldLayout = oldImageLayout;
    barrier.newLayout = newImageLayout;
    barrier.image = image;
    barrier.subresourceRange = subresourceRange;

    switch (oldImageLayout)
    {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            barrier.srcAccessMask = 0;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        default:
            break;
    }

    switch (newImageLayout)
    {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            if (barrier.srcAccessMask == 0)
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        default:
            break;
    }

    return barrier;
}
