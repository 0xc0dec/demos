/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#include <algorithm>
#include "VulkanMesh.h"
#include "VulkanDevice.h"

vk::Mesh::Mesh(Device *device):
    device_(device)
{
}

void vk::Mesh::addVertexBuffer(const VertexBufferLayout &layout, const std::vector<float> &data, uint32_t vertexCount)
{
	vertexBuffers_.push_back(Buffer::deviceLocal(*device_, layout.size() * vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, data.data()));
    layouts_.push_back(layout);
    vertexCounts_.push_back(vertexCount);
}

void vk::Mesh::addDynamicVertexBuffer(const VertexBufferLayout &layout, const std::vector<float> &data, uint32_t vertexCount)
{
	vertexBuffers_.push_back(Buffer::hostVisible(*device_, layout.size() * vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, data.data()));
    layouts_.push_back(layout);
    vertexCounts_.push_back(vertexCount);
}

void vk::Mesh::updateVertexBuffer(uint32_t index, uint32_t vertexOffset, const void *data, uint32_t vertexCount)
{
    const auto vertexSize = layouts_[index].size();
    vertexBuffers_[index].updatePart(data, vertexOffset * vertexSize, vertexCount * vertexSize);
}

void vk::Mesh::addIndexBuffer(const std::vector<uint32_t> &data, uint32_t elementCount)
{
    const auto size = static_cast<VkDeviceSize>(sizeof(uint32_t)) * elementCount;
    auto buf = Buffer::deviceLocal(*device_, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, data.data());
    indexBuffers_.push_back(std::move(buf));
    indexElementCounts_.push_back(elementCount);
}
