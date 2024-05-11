/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include "../VertexBufferLayout.h"
#include "VulkanBuffer.h"
#include "VulkanPipeline.h"

namespace vk
{
    class Device;

    class Mesh
    {
    public:
        explicit Mesh(Device *device);
        ~Mesh() = default;

        void addVertexBuffer(const VertexBufferLayout &layout, const std::vector<float> &data, uint32_t vertexCount);
        void addDynamicVertexBuffer(const VertexBufferLayout &layout, const std::vector<float> &data, uint32_t vertexCount);
        void updateVertexBuffer(uint32_t index, uint32_t vertexOffset, const void *data, uint32_t vertexCount);
        auto vertexBuffer(uint32_t index) const -> VkBuffer { return vertexBuffers_.at(index).handle(); }

        void addIndexBuffer(const std::vector<uint32_t> &data, uint32_t elementCount);
        auto indexBuffer(uint32_t index) const -> VkBuffer { return indexBuffers_.at(index).handle(); }

    private:
        Device *device_;
        std::vector<VertexBufferLayout> layouts_;
        std::vector<uint32_t> vertexCounts_;
        std::vector<uint32_t> indexElementCounts_;
        std::vector<vk::Buffer> vertexBuffers_;
        std::vector<vk::Buffer> indexBuffers_;
    };
}
