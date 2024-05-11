/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include "VulkanResource.h"

namespace vk
{
    class Device;

    class Buffer
    {
    public:
        static auto staging(const Device &dev, VkDeviceSize size, const void *initialData = nullptr) -> Buffer;
        static auto uniformHostVisible(const Device &dev, VkDeviceSize size) -> Buffer;
        static auto deviceLocal(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, const void *data) -> Buffer;
        static auto hostVisible(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, const void *data) -> Buffer;

        Buffer() = default;
        Buffer(const Device &dev, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memPropertyFlags);
        Buffer(Buffer &&other) = default;
        Buffer(const Buffer &other) = delete;
        ~Buffer() = default;

        auto operator=(const Buffer &other) -> Buffer & = delete;
        auto operator=(Buffer &&other) -> Buffer & = default;

        operator VkBuffer() const { return buffer_; }

        auto handle() const -> VkBuffer { return buffer_; }
        auto size() const -> VkDeviceSize { return size_; }

        void updateAll(const void *newData) const;
        void updatePart(const void *newData, uint32_t offset, uint32_t size) const;
        void transferTo(const Buffer &dst) const;

    private:
        const Device *device_ = nullptr;
        Resource<VkDeviceMemory> memory_;
        Resource<VkBuffer> buffer_;
        VkDeviceSize size_ = 0;
    };
}