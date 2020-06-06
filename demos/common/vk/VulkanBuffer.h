/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include "VulkanResource.h"

namespace vk
{
	class Device;
	
	class VulkanBuffer
    {
    public:
        static auto staging(const Device &dev, VkDeviceSize size, const void *initialData = nullptr) -> VulkanBuffer;
        static auto uniformHostVisible(const Device &dev, VkDeviceSize size) -> VulkanBuffer;
        static auto deviceLocal(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, const void *data) -> VulkanBuffer;
        static auto hostVisible(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, const void *data) -> VulkanBuffer;

        VulkanBuffer() = default;
        VulkanBuffer(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memPropertyFlags);
        VulkanBuffer(VulkanBuffer &&other) = default;
        VulkanBuffer(const VulkanBuffer &other) = delete;
        ~VulkanBuffer() = default;

        auto operator=(const VulkanBuffer &other) -> VulkanBuffer& = delete;
        auto operator=(VulkanBuffer &&other) -> VulkanBuffer& = default;

        operator VkBuffer() const { return buffer_; }

        auto handle() const -> VkBuffer { return buffer_; }
        auto size() const -> VkDeviceSize { return size_; }

        void updateAll(const void *newData) const;
        void updatePart(const void *newData, uint32_t offset, uint32_t size) const;
        void transferTo(const VulkanBuffer& dst) const;

    private:
        const Device *device_ = nullptr;
        Resource<VkDeviceMemory> memory_;
        Resource<VkBuffer> buffer_;
        VkDeviceSize size_ = 0;
    };
}