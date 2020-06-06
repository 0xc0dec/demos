/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VulkanBuffer.h"
#include "VulkanCommon.h"
#include "VulkanDevice.h"

auto vk::VulkanBuffer::staging(const Device &dev, VkDeviceSize size, const void *initialData) -> VulkanBuffer
{
    auto buffer = VulkanBuffer(dev, size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (initialData)
        buffer.updateAll(initialData);

    return buffer;
}

auto vk::VulkanBuffer::uniformHostVisible(const Device &dev, VkDeviceSize size) -> VulkanBuffer
{
    return VulkanBuffer(dev, size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

auto vk::VulkanBuffer::deviceLocal(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, const void *data) -> VulkanBuffer
{
	const auto stagingBuffer = staging(dev, size, data);
    auto buffer = VulkanBuffer(dev, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    stagingBuffer.transferTo(buffer);
    return buffer;
}

auto vk::VulkanBuffer::hostVisible(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, const void *data) -> VulkanBuffer
{
    auto buffer = VulkanBuffer(dev, size, usageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    buffer.updateAll(data);
    return buffer;
}

vk::VulkanBuffer::VulkanBuffer(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memPropertyFlags):
    device_(&dev),
    size_(size)
{
    VkBufferCreateInfo bufferInfo {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.flags = 0;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = nullptr;

    buffer_ = Resource<VkBuffer>{dev.handle(), vkDestroyBuffer};
    vk::assertResult(vkCreateBuffer(dev.handle(), &bufferInfo, nullptr, buffer_.cleanRef()));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(dev.handle(), buffer_, &memReqs);

    VkMemoryAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = vk::findMemoryType(dev.physicalMemoryFeatures(), memReqs.memoryTypeBits, memPropertyFlags);

    memory_ = Resource<VkDeviceMemory>{dev.handle(), vkFreeMemory};
    vk::assertResult(vkAllocateMemory(dev.handle(), &allocInfo, nullptr, memory_.cleanRef()));
    vk::assertResult(vkBindBufferMemory(dev.handle(), buffer_, memory_, 0));
}

void vk::VulkanBuffer::updateAll(const void *newData) const
{
    void *ptr = nullptr;
    vk::assertResult(vkMapMemory(device_->handle(), memory_, 0, VK_WHOLE_SIZE, 0, &ptr));
    memcpy(ptr, newData, size_);
    vkUnmapMemory(device_->handle(), memory_);
}

void vk::VulkanBuffer::updatePart(const void *newData, uint32_t offset, uint32_t size) const
{
    void *ptr = nullptr;
    vk::assertResult(vkMapMemory(device_->handle(), memory_, offset, VK_WHOLE_SIZE, 0, &ptr));
    memcpy(ptr, newData, size);
    vkUnmapMemory(device_->handle(), memory_);
}

void vk::VulkanBuffer::transferTo(const VulkanBuffer &dst) const
{
    VulkanCmdBuffer(*device_)
        .begin(true)
        .copyBuffer(*this, dst)
        .endAndFlush();
}