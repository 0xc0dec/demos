/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VulkanRenderPass.h"
#include <array>

using namespace vk;

vk::RenderPass::RenderPass(VkDevice device, const RenderPassConfig &config)
{
    const auto colorAttachments = config.colorAttachmentRefs_.empty() ? nullptr : config.colorAttachmentRefs_.data();
    const auto depthAttachment = config.depthAttachmentRef_.layout != VK_IMAGE_LAYOUT_UNDEFINED ? &config.depthAttachmentRef_ : nullptr;
    clearValues_.resize(config.colorAttachmentRefs_.size() + 1);
    clearValues_.rbegin()->depthStencil = {1, 0};

    colorAttachmentCount_ = config.colorAttachmentRefs_.size();

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = config.colorAttachmentRefs_.size();
    subpass.pColorAttachments = colorAttachments;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = depthAttachment;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.flags = 0;
    renderPassInfo.pNext = nullptr;
    renderPassInfo.attachmentCount = config.attachments_.size();
    renderPassInfo.pAttachments = config.attachments_.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();

    pass_ = Resource<VkRenderPass>{device, vkDestroyRenderPass};
    vk::assertResult(vkCreateRenderPass(device, &renderPassInfo, nullptr, pass_.cleanRef()));
}

void RenderPass::begin(VkCommandBuffer cmdBuf, VkFramebuffer framebuffer, uint32_t canvasWidth, uint32_t canvasHeight)
{
    VkRenderPassBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.pNext = nullptr;
    info.renderPass = pass_;
    info.renderArea.offset.x = 0;
    info.renderArea.offset.y = 0;
    info.renderArea.extent.width = canvasWidth;
    info.renderArea.extent.height = canvasHeight;
    info.clearValueCount = clearValues_.size();
    info.pClearValues = clearValues_.data();
    info.framebuffer = framebuffer;

    vkCmdBeginRenderPass(cmdBuf, &info, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::end(VkCommandBuffer cmdBuf)
{
    vkCmdEndRenderPass(cmdBuf);
}

RenderPassConfig::RenderPassConfig():
    depthAttachmentRef_{0, VK_IMAGE_LAYOUT_UNDEFINED}
{
}

auto RenderPassConfig::addColorAttachment(VkFormat format, VkImageLayout finalLayout)
    -> RenderPassConfig&
{
    VkAttachmentDescription desc{};
    desc.format = format;
    desc.flags = 0;
    desc.samples = VK_SAMPLE_COUNT_1_BIT;
    desc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    desc.finalLayout = finalLayout;
    attachments_.push_back(desc);

    VkAttachmentReference reference{};
    reference.attachment = attachments_.size() - 1;
    reference.layout = VK_IMAGE_LAYOUT_GENERAL; // TODO better
    colorAttachmentRefs_.push_back(reference);

    return *this;
}

auto RenderPassConfig::setDepthAttachment(VkFormat format) -> RenderPassConfig&
{
    VkAttachmentDescription desc{};
    desc.format = format;
    desc.flags = 0;
    desc.samples = VK_SAMPLE_COUNT_1_BIT;
    desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    desc.finalLayout = VK_IMAGE_LAYOUT_GENERAL; // TODO better
    attachments_.push_back(desc);

    depthAttachmentRef_.attachment = attachments_.size() - 1;
    depthAttachmentRef_.layout = VK_IMAGE_LAYOUT_GENERAL; // TODO better

    return *this;
}
