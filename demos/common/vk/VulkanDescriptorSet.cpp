/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#include "VulkanDescriptorSet.h"

using namespace vk;

void DescriptorSetConfig::addUniformBuffer(uint32_t binding)
{
    VkDescriptorSetLayoutBinding b{};
    b.binding = binding;
    b.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    b.descriptorCount = 1;
    b.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS; // TODO make configurable
    b.pImmutableSamplers = nullptr;
    bindings_.push_back(b);
    // TODO More elegant
    sizes_[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sizes_[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER].descriptorCount++;
}

void DescriptorSetConfig::addSampler(uint32_t binding)
{
    VkDescriptorSetLayoutBinding b{};
    b.binding = binding;
    b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    b.descriptorCount = 1;
    b.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // TODO make configurable
    b.pImmutableSamplers = nullptr;
    bindings_.push_back(b);
    // TODO More elegant
    sizes_[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    sizes_[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER].descriptorCount++;
}

DescriptorSet::DescriptorSet(VkDevice device, const DescriptorSetConfig &cfg):
    device_(device)
{
    // Layout
    VkDescriptorSetLayoutCreateInfo layoutInfo {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = cfg.bindings_.size();
    layoutInfo.pBindings = cfg.bindings_.data();

    layout_ = Resource<VkDescriptorSetLayout>{device, vkDestroyDescriptorSetLayout};
    vk::assertResult(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, layout_.cleanRef()));

    std::vector<VkDescriptorPoolSize> sizes;
    for (const auto &s: cfg.sizes_)
    {
        if (s.second.descriptorCount > 0)
            sizes.push_back(s.second);
    }

    // Pool
    // TODO support shared pools
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = sizes.size();
    poolInfo.pPoolSizes = sizes.data();
    poolInfo.maxSets = 1;

    pool_ = Resource<VkDescriptorPool>{device, vkDestroyDescriptorPool};
    vk::assertResult(vkCreateDescriptorPool(device, &poolInfo, nullptr, pool_.cleanRef()));

    // Set
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool_;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout_;

    vk::assertResult(vkAllocateDescriptorSets(device, &allocInfo, &set_));
}

// TODO do updates in batch using single vkUpdateDescriptorSets call
void DescriptorSet::updateUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) const
{
    VkDescriptorBufferInfo bufferInfo = {buffer, offset, range};

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set_;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufferInfo;
    write.pImageInfo = nullptr;
    write.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device_, 1, &write, 0, nullptr);
}

// TODO do updates in batch using single vkUpdateDescriptorSets call
void DescriptorSet::updateSampler(uint32_t binding, VkImageView view, VkSampler sampler, VkImageLayout layout) const
{
    VkDescriptorImageInfo imageInfo = {sampler, view, layout};

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set_;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pBufferInfo = nullptr;
    write.pImageInfo = &imageInfo;
    write.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device_, 1, &write, 0, nullptr);
}
