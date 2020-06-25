/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "VulkanCommon.h"
#include <vector>

namespace vk
{
    class RenderPassConfig
    {
    public:
        RenderPassConfig();

        auto addColorAttachment(VkFormat colorFormat, VkImageLayout finalLayout) -> RenderPassConfig&;
        auto setDepthAttachment(VkFormat depthFormat) -> RenderPassConfig&;

    private:
        friend class RenderPass;

        std::vector<VkAttachmentDescription> attachments_;
        std::vector<VkAttachmentReference> colorAttachmentRefs_;
        VkAttachmentReference depthAttachmentRef_;
    };

    class RenderPass
    {
    public:
        RenderPass() = default;
        RenderPass(VkDevice device, const RenderPassConfig &config);
        RenderPass(const RenderPass &other) = delete;
        RenderPass(RenderPass &&other) = default;
        ~RenderPass() = default;

        auto clearValues() const -> const std::vector<VkClearValue>& { return clearValues_; }
        auto colorAttachmentCount() const -> uint32_t { return colorAttachmentCount_; }

        void begin(VkCommandBuffer cmdBuf, VkFramebuffer framebuffer, uint32_t canvasWidth, uint32_t canvasHeight);
        void end(VkCommandBuffer cmdBuf);

        auto operator=(const RenderPass &other) -> RenderPass& = delete;
        auto operator=(RenderPass &&other) -> RenderPass& = default;

    	auto handle() const -> VkRenderPass { return pass_; }

        operator const VkRenderPass() const { return pass_; }

    private:
        Resource<VkRenderPass> pass_;
        std::vector<VkClearValue> clearValues_;
        uint32_t colorAttachmentCount_ = 0;
    };
}
