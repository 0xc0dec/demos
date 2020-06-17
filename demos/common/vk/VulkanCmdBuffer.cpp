/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#include "VulkanCmdBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanDevice.h"

using namespace vk;

vk::CmdBuffer::CmdBuffer(const Device &dev):
    device_(&dev)
{
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = dev.commandPool();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    handle_ = Resource<VkCommandBuffer>{dev.handle(), dev.commandPool(), vkFreeCommandBuffers};
    vk::assertResult(vkAllocateCommandBuffers(dev.handle(), &allocateInfo, &handle_));
}

auto CmdBuffer::beginRenderPass(const RenderPass &pass, VkFramebuffer framebuffer, uint32_t canvasWidth,
    uint32_t canvasHeight) -> CmdBuffer&
{
    VkRenderPassBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.pNext = nullptr;
    info.renderPass = pass;
    info.renderArea.offset.x = 0;
    info.renderArea.offset.y = 0;
    info.renderArea.extent.width = canvasWidth;
    info.renderArea.extent.height = canvasHeight;
    info.clearValueCount = pass.clearValues().size();
    info.pClearValues = pass.clearValues().data();
    info.framebuffer = framebuffer;

    vkCmdBeginRenderPass(handle_, &info, VK_SUBPASS_CONTENTS_INLINE);

    return *this;
}

auto CmdBuffer::endRenderPass() -> CmdBuffer&
{
    vkCmdEndRenderPass(handle_);
    return *this;
}

auto CmdBuffer::bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
    -> CmdBuffer&
{
    vkCmdBindIndexBuffer(handle_, buffer, offset, indexType);
    return *this;
}

auto CmdBuffer::bindVertexBuffer(uint32_t binding, VkBuffer buffer) -> CmdBuffer&
{
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(handle_, binding, 1, &buffer, &offset);
    return *this;
}

auto CmdBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
    uint32_t vertexOffset, uint32_t firstInstance) -> CmdBuffer&
{
    vkCmdDrawIndexed(handle_, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    return *this;
}

auto CmdBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    -> CmdBuffer&
{
    vkCmdDraw(handle_, vertexCount, instanceCount, firstVertex, firstInstance);
    return *this;
}

auto CmdBuffer::bindPipeline(VkPipeline pipeline) -> CmdBuffer&
{
    vkCmdBindPipeline(handle_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    return *this;
}

auto CmdBuffer::bindDescriptorSet(VkPipelineLayout pipelineLayout, const DescriptorSet &set)
    -> CmdBuffer&
{
    vkCmdBindDescriptorSets(handle_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, set, 0, nullptr);
    return *this;
}

auto CmdBuffer::setViewport(const glm::vec4 &dimentions, float minDepth, float maxDepth) -> CmdBuffer&
{
    VkViewport vp{dimentions.x(), dimentions.y(), dimentions.z(), dimentions.w(), minDepth, maxDepth};
    vkCmdSetViewport(handle_, 0, 1, &vp);
    return *this;
}

auto CmdBuffer::setScissor(const glm::vec4 &dimentions) -> CmdBuffer&
{
    VkRect2D scissor{{0, 0}, {static_cast<uint32_t>(dimentions.z()), static_cast<uint32_t>(dimentions.w())}};
    // TODO proper values
    vkCmdSetScissor(handle_, 0, 1, &scissor);
    return *this;
}

auto CmdBuffer::putImagePipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
    const VkImageMemoryBarrier &barrier) -> CmdBuffer&
{
    vkCmdPipelineBarrier(handle_, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    return *this;
}

auto CmdBuffer::clearColorAttachment(uint32_t attachment, const VkClearValue &clearValue, const VkClearRect &clearRect)
    -> CmdBuffer&
{
    VkClearAttachment clear;
    clear.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    clear.colorAttachment = attachment;
    clear.clearValue = clearValue;
    vkCmdClearAttachments(handle_, 1, &clear, 1, &clearRect); // TODO in batch
    return *this;
}

auto CmdBuffer::copyBuffer(const Buffer &src, const Buffer &dst) -> CmdBuffer&
{
    VkBufferCopy copyRegion{};
    copyRegion.size = dst.size();
    vkCmdCopyBuffer(handle_, src.handle(), dst.handle(), 1, &copyRegion);
    return *this;
}

auto CmdBuffer::copyBuffer(const Buffer &src, const VulkanImage &dst) -> CmdBuffer&
{
    VkBufferImageCopy bufferCopyRegion{};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.mipLevel = 0;
    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = dst.width();
    bufferCopyRegion.imageExtent.height = dst.height();
    bufferCopyRegion.imageExtent.depth = 1;
    bufferCopyRegion.bufferOffset = 0;

    vkCmdCopyBufferToImage(
        handle_,
        src.handle(),
        dst.handle(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &bufferCopyRegion);

    return *this;
}

auto CmdBuffer::copyBuffer(const Buffer &src, const VulkanImage &dst, const VkBufferImageCopy *regions,
    uint32_t regionCount) -> CmdBuffer&
{
    vkCmdCopyBufferToImage(
        handle_,
        src.handle(),
        dst.handle(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        regionCount,
        regions);
    return *this;
}

auto CmdBuffer::blit(VkImage src, VkImage dst, VkImageLayout srcLayout, VkImageLayout dstLayout,
    const VkImageBlit &blit, VkFilter filter) -> CmdBuffer&
{
    vkCmdBlitImage(handle_,
        src, srcLayout,
        dst, dstLayout,
        1, &blit,
        filter);
    return *this;
}

void CmdBuffer::endAndFlush()
{
    end();
    vk::queueSubmit(device_->queue(), 0, nullptr, 0, nullptr, 1, &handle_);
    vk::assertResult(vkQueueWaitIdle(device_->queue()));
}

auto CmdBuffer::begin(bool transient) -> CmdBuffer&
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = transient ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;
    vk::assertResult(vkBeginCommandBuffer(handle_, &beginInfo));
    return *this;
}

void CmdBuffer::end() const
{
    vk::assertResult(vkEndCommandBuffer(handle_));
}
