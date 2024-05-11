/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "VulkanBuffer.h"
#include "VulkanCmdBuffer.h"
#include "VulkanCommon.h"
#include "VulkanDevice.h"

auto vk::Buffer::staging(const Device &dev, VkDeviceSize size, const void *initialData) -> Buffer
{
    auto buffer = Buffer(dev, size,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (initialData)
        buffer.updateAll(initialData);

    return buffer;
}

auto vk::Buffer::uniformHostVisible(const Device &dev, VkDeviceSize size) -> Buffer
{
    return Buffer(dev, size,
                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

auto vk::Buffer::deviceLocal(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, const void *data) -> Buffer
{
    const auto stagingBuffer = staging(dev, size, data);
    auto buffer = Buffer(dev, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    stagingBuffer.transferTo(buffer);
    return buffer;
}

auto vk::Buffer::hostVisible(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, const void *data) -> Buffer
{
    auto buffer = Buffer(dev, size, usageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    buffer.updateAll(data);
    return buffer;
}

vk::Buffer::Buffer(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memPropertyFlags) : device_(&dev),
                                                                                                                                  size_(size)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.flags = 0;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = nullptr;

    buffer_ = Resource<VkBuffer>{dev.handle(), vkDestroyBuffer};
    vk::ensure(vkCreateBuffer(dev.handle(), &bufferInfo, nullptr, buffer_.cleanRef()));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(dev.handle(), buffer_, &memReqs);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = vk::findMemoryType(dev.physicalMemoryFeatures(), memReqs.memoryTypeBits, memPropertyFlags);

    memory_ = Resource<VkDeviceMemory>{dev.handle(), vkFreeMemory};
    vk::ensure(vkAllocateMemory(dev.handle(), &allocInfo, nullptr, memory_.cleanRef()));
    vk::ensure(vkBindBufferMemory(dev.handle(), buffer_, memory_, 0));
}

void vk::Buffer::updateAll(const void *newData) const
{
    void *ptr = nullptr;
    vk::ensure(vkMapMemory(device_->handle(), memory_, 0, VK_WHOLE_SIZE, 0, &ptr));
    memcpy(ptr, newData, size_);
    vkUnmapMemory(device_->handle(), memory_);
}

void vk::Buffer::updatePart(const void *newData, uint32_t offset, uint32_t size) const
{
    void *ptr = nullptr;
    vk::ensure(vkMapMemory(device_->handle(), memory_, offset, VK_WHOLE_SIZE, 0, &ptr));
    memcpy(ptr, newData, size);
    vkUnmapMemory(device_->handle(), memory_);
}

void vk::Buffer::transferTo(const Buffer &dst) const
{
    CmdBuffer(*device_)
        .begin(true)
        .copyBuffer(*this, dst)
        .endAndFlush();
}
