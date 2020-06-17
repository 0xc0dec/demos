/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#pragma once

#include <glm/vec4.hpp>
#include "VulkanCommon.h"

namespace vk
{
    class RenderPass;
    class DescriptorSet;
    class Buffer;
    class Device;

    class CmdBuffer
    {
    public:
        CmdBuffer() = default;
        CmdBuffer(const Device &dev);
        CmdBuffer(const CmdBuffer &other) = delete;
        CmdBuffer(CmdBuffer &&other) = default;
        ~CmdBuffer() = default;

        auto begin(bool transient) -> CmdBuffer&; // TODO better understanding of the `transient` param
        void end() const;
        void endAndFlush();

        auto beginRenderPass(const RenderPass &pass, VkFramebuffer framebuffer, uint32_t canvasWidth, uint32_t canvasHeight)
            -> CmdBuffer&;
        auto endRenderPass() -> CmdBuffer&;

        auto bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) -> CmdBuffer&;
        auto bindVertexBuffer(uint32_t binding, VkBuffer buffer) -> CmdBuffer&;
        auto drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
            -> CmdBuffer&;
        auto draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) -> CmdBuffer&;

        auto bindPipeline(VkPipeline pipeline) -> CmdBuffer&;
        auto bindDescriptorSet(VkPipelineLayout pipelineLayout, const DescriptorSet &set) -> CmdBuffer&;

        auto setViewport(const glm::vec4 &dimentions, float minDepth, float maxDepth) -> CmdBuffer&;
        auto setScissor(const glm::vec4 &dimentions) -> CmdBuffer&;

        auto putImagePipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
            const VkImageMemoryBarrier &barrier) -> CmdBuffer&;

        auto clearColorAttachment(uint32_t attachment, const VkClearValue &clearValue, const VkClearRect &clearRect)
            -> CmdBuffer&;

        auto copyBuffer(const Buffer &src, const Buffer &dst) -> CmdBuffer&;
        auto copyBuffer(const Buffer &src, const VulkanImage &dst) -> CmdBuffer&;
        auto copyBuffer(const Buffer &src, const VulkanImage &dst, 
            const VkBufferImageCopy *regions, uint32_t regionCount) -> CmdBuffer&;

        auto blit(VkImage src, VkImage dst, VkImageLayout srcLayout, VkImageLayout dstLayout,
            const VkImageBlit &blit, VkFilter filter) -> CmdBuffer&;

        auto operator=(const CmdBuffer &other) -> CmdBuffer& = delete;
        auto operator=(CmdBuffer &&other) -> CmdBuffer& = default;

        operator bool() const { return handle_; }
    	operator VkCommandBuffer() const { return handle_; }
        operator const VkCommandBuffer*() { return &handle_; }

    private:
        const Device *device_ = nullptr;
        Resource<VkCommandBuffer> handle_;
    };
}
