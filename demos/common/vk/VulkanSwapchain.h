/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include "VulkanCommon.h"
#include "VulkanImage.h"
#include "VulkanRenderPass.h"

namespace vk
{
    class Device;

    class Swapchain final
    {
    public:
        Swapchain() = default;
        Swapchain(const Device &dev, uint32_t width, uint32_t height, bool vsync);
        Swapchain(const Swapchain &other) = delete;
        Swapchain(Swapchain &&other) = default;
        ~Swapchain() = default;

        auto operator=(const Swapchain &other) -> Swapchain & = delete;
        auto operator=(Swapchain &&other) -> Swapchain & = default;

        operator VkSwapchainKHR() { return swapchain_; }
        operator VkSwapchainKHR() const { return swapchain_; }

        auto currentFrameBuffer() -> VkFramebuffer { return steps_[currentStep_].framebuffer; }
        auto renderPass() -> RenderPass & { return renderPass_; }

        auto moveNext() -> VkSemaphore;
        void present(VkQueue queue, uint32_t waitSemaphoreCount, const VkSemaphore *waitSemaphores);

        auto imageCount() const -> uint32_t { return steps_.size(); }

    private:
        struct Step
        {
            Resource<VkImageView> imageView;
            Resource<VkFramebuffer> framebuffer;
        };

        VkDevice device_ = nullptr;
        Resource<VkSwapchainKHR> swapchain_;
        Image depthStencil_;
        std::vector<Step> steps_;
        Resource<VkSemaphore> presentCompleteSem_;
        RenderPass renderPass_;
        uint32_t currentStep_ = 0;
    };
}
