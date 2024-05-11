/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#pragma once

#include <unordered_map>
#include "VulkanCommon.h"

namespace vk
{
    class Material;

    class DescriptorSetConfig
    {
    public:
        void addUniformBuffer(uint32_t binding);
        void addSampler(uint32_t binding);

    private:
        friend class DescriptorSet;

        std::vector<VkDescriptorSetLayoutBinding> bindings_;
        std::unordered_map<VkDescriptorType, VkDescriptorPoolSize> sizes_;
    };

    class DescriptorSet
    {
    public:
        DescriptorSet() = default;
        DescriptorSet(VkDevice device, const DescriptorSetConfig &cfg);
        DescriptorSet(DescriptorSet &&other) = default;
        DescriptorSet(const DescriptorSet &other) = delete;
        ~DescriptorSet() = default;

        auto layout() const -> VkDescriptorSetLayout { return layout_; }

        void updateUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) const;
        void updateSampler(uint32_t binding, VkImageView view, VkSampler sampler, VkImageLayout layout) const;

        auto operator=(const DescriptorSet &other) -> DescriptorSet & = delete;
        auto operator=(DescriptorSet &&other) -> DescriptorSet & = default;

        operator bool() const { return set_ != VK_NULL_HANDLE; }
        operator const VkDescriptorSet *() const { return &set_; }

    private:
        VkDevice device_ = VK_NULL_HANDLE;
        Resource<VkDescriptorPool> pool_;
        Resource<VkDescriptorSetLayout> layout_;
        VkDescriptorSet set_ = VK_NULL_HANDLE;
    };
}
