/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#pragma once

#include "VulkanCommon.h"

namespace vk
{
    class VulkanPipelineConfig
    {
    public:
        VulkanPipelineConfig(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        ~VulkanPipelineConfig() = default;

        auto withColorBlendAttachmentCount(uint32_t count) -> VulkanPipelineConfig&;
        auto withVertexAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset) -> VulkanPipelineConfig&;
        auto withVertexBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate) -> VulkanPipelineConfig&;
        auto withDescriptorSetLayout(VkDescriptorSetLayout layout) -> VulkanPipelineConfig&;
        auto withFrontFace(VkFrontFace frontFace) -> VulkanPipelineConfig&;
        auto withCullMode(VkCullModeFlags cullFlags) -> VulkanPipelineConfig&;
        auto withDepthTest(bool write, bool test) -> VulkanPipelineConfig&;
        auto withBlend(bool enabled, VkBlendFactor srcColorFactor, VkBlendFactor dstColorFactor,
            VkBlendFactor srcAlphaFactor, VkBlendFactor dstAlphaFactor) -> VulkanPipelineConfig&;
        auto withTopology(VkPrimitiveTopology topology) -> VulkanPipelineConfig&;
        auto withPolygonMode(VkPolygonMode mode) -> VulkanPipelineConfig&;

    private:
        friend class Pipeline;

        VkShaderModule vs_;
        VkShaderModule fs_;
        VkPipelineRasterizationStateCreateInfo rasterStateInfo_;
        VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo_;
        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates_;

        std::vector<VkVertexInputAttributeDescription> vertexAttrs_;
        std::vector<VkVertexInputBindingDescription> vertexBindings_;
        std::vector<VkDescriptorSetLayout> descSetLayouts_;

        VkPrimitiveTopology topology_ = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    };

    class Pipeline
    {
    public:
        Pipeline() = default;
        Pipeline(VkDevice device, VkRenderPass renderPass, const VulkanPipelineConfig &config);
        Pipeline(const Pipeline &other) = delete;
        Pipeline(Pipeline &&other) = default;
        ~Pipeline() = default;

        auto operator=(const Pipeline &other) -> Pipeline& = delete;
        auto operator=(Pipeline &&other) -> Pipeline& = default;

        operator VkPipeline() const { return pipeline_; }

        auto handle() const -> VkPipeline { return pipeline_; }
        auto layout() const -> VkPipelineLayout { return layout_; }

    private:
        Resource<VkPipeline> pipeline_;
        Resource<VkPipelineLayout> layout_;
    };

    inline auto VulkanPipelineConfig::withTopology(VkPrimitiveTopology topology) -> VulkanPipelineConfig&
    {
        this->topology_ = topology;
        return *this;
    }

    inline auto VulkanPipelineConfig::withPolygonMode(VkPolygonMode mode) -> VulkanPipelineConfig&
    {
        rasterStateInfo_.polygonMode = mode;
        return *this;
    }

    inline auto VulkanPipelineConfig::withDescriptorSetLayout(VkDescriptorSetLayout layout) -> VulkanPipelineConfig&
    {
        descSetLayouts_.push_back(layout);
        return *this;
    }

    inline auto VulkanPipelineConfig::withFrontFace(VkFrontFace frontFace) -> VulkanPipelineConfig&
    {
        rasterStateInfo_.frontFace = frontFace;
        return *this;
    }

    inline auto VulkanPipelineConfig::withCullMode(VkCullModeFlags cullFlags) -> VulkanPipelineConfig&
    {
        rasterStateInfo_.cullMode = cullFlags;
        return *this;
    }
}
