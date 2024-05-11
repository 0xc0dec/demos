/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanCmdBuffer.h"

static auto getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain) -> std::vector<VkImage>
{
    uint32_t imageCount = 0;
    vk::ensure(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));

    std::vector<VkImage> images;
    images.resize(imageCount);
    vk::ensure(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data()));

    return images;
}

static auto selectPresentMode(const vk::Device &dev, bool vsync) -> VkPresentModeKHR
{
    uint32_t presentModeCount = 0;
    vk::ensure(vkGetPhysicalDeviceSurfacePresentModesKHR(dev.physical(), dev.surface(), &presentModeCount, nullptr));

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vk::ensure(vkGetPhysicalDeviceSurfacePresentModesKHR(dev.physical(), dev.surface(), &presentModeCount, presentModes.data()));

    auto presentMode = VK_PRESENT_MODE_FIFO_KHR; // "vsync"

    if (!vsync)
    {
        for (const auto mode : presentModes)
        {
            if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                presentMode = mode;
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                presentMode = mode;
                break;
            }
        }
    }

    return presentMode;
}

static auto createSwapchain(const vk::Device &dev, uint32_t width, uint32_t height, bool vsync) -> vk::Resource<VkSwapchainKHR>
{
    VkSurfaceCapabilitiesKHR capabilities;
    vk::ensure(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev.physical(), dev.surface(), &capabilities));

    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
    {
        width = capabilities.currentExtent.width;
        height = capabilities.currentExtent.height;
    }

    VkSurfaceTransformFlagsKHR transformFlags;
    if (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        transformFlags = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else
        transformFlags = capabilities.currentTransform;

    auto requestedImageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && requestedImageCount > capabilities.maxImageCount)
        requestedImageCount = capabilities.maxImageCount;

    const auto presentMode = selectPresentMode(dev, vsync);

    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.pNext = nullptr;
    swapchainInfo.surface = dev.surface();
    swapchainInfo.minImageCount = requestedImageCount;
    swapchainInfo.imageFormat = dev.colorFormat();
    swapchainInfo.imageColorSpace = dev.colorSpace();
    swapchainInfo.imageExtent = {width, height};
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(transformFlags);
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices = nullptr;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    vk::Resource<VkSwapchainKHR> swapchain{dev.handle(), vkDestroySwapchainKHR};
    vk::ensure(vkCreateSwapchainKHR(dev.handle(), &swapchainInfo, nullptr, swapchain.cleanRef()));

    return swapchain;
}

vk::Swapchain::Swapchain(const Device &dev, uint32_t width, uint32_t height, bool vsync) : device_(dev.handle())
{
    const auto colorFormat = dev.colorFormat();
    const auto depthFormat = dev.depthFormat();

    swapchain_ = createSwapchain(dev, width, height, vsync);

    renderPass_ = RenderPass(this->device_, RenderPassConfig()
                                                .addColorAttachment(colorFormat, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
                                                .setDepthAttachment(depthFormat));

    depthStencil_ = Image::swapchainDepthStencil(dev, width, height, depthFormat);

    auto cmdBuf = CmdBuffer(dev);
    cmdBuf.begin(true);

    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.layerCount = 1;

    auto images = getSwapchainImages(this->device_, swapchain_);
    steps_.resize(images.size());

    for (uint32_t i = 0; i < images.size(); i++)
    {
        // TODO Use FrameBuffer class here?
        steps_[i].imageView = vk::createImageView(this->device_, colorFormat, VK_IMAGE_VIEW_TYPE_2D, 1, 1, images[i], VK_IMAGE_ASPECT_COLOR_BIT);
        steps_[i].framebuffer = vk::createFrameBuffer(this->device_, {steps_[i].imageView, depthStencil_.view()}, renderPass_, width, height);

        cmdBuf.putImagePipelineBarrier(
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            vk::makeImagePipelineBarrier(
                images[i],
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                range));
    }

    cmdBuf.endAndFlush();

    presentCompleteSem_ = vk::createSemaphore(this->device_);
}

auto vk::Swapchain::moveNext() -> VkSemaphore
{
    vk::ensure(vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, presentCompleteSem_, VK_NULL_HANDLE, &currentStep_));
    return presentCompleteSem_;
}

void vk::Swapchain::present(VkQueue queue, uint32_t waitSemaphoreCount, const VkSemaphore *waitSemaphores)
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain_;
    presentInfo.pImageIndices = &currentStep_;
    presentInfo.pWaitSemaphores = waitSemaphores;
    presentInfo.waitSemaphoreCount = waitSemaphoreCount;
    vk::ensure(vkQueuePresentKHR(queue, &presentInfo));
}
