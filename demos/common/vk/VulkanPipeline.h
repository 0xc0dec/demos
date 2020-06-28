/* 
 * Copyright (c) Aleksey Fedotov 
 * MIT license 
 */

#pragma once

#include "VulkanCommon.h"

namespace vk
{
    class PipelineConfig
    {
    public:
        PipelineConfig(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        ~PipelineConfig() = default;

        auto withColorBlendAttachmentCount(uint32_t count) -> PipelineConfig&;
        auto withVertexAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset) -> PipelineConfig&;
        auto withVertexBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate) -> PipelineConfig&;
        auto withDescriptorSetLayout(VkDescriptorSetLayout layout) -> PipelineConfig&;
        auto withFrontFace(VkFrontFace frontFace) -> PipelineConfig&;
        auto withCullMode(VkCullModeFlags cullFlags) -> PipelineConfig&;
        auto withDepthTest(bool write, bool test) -> PipelineConfig&;
        auto withBlend(bool enabled, VkBlendFactor srcColorFactor, VkBlendFactor dstColorFactor,
            VkBlendFactor srcAlphaFactor, VkBlendFactor dstAlphaFactor) -> PipelineConfig&;
        auto withTopology(VkPrimitiveTopology topology) -> PipelineConfig&;
        auto withPolygonMode(VkPolygonMode mode) -> PipelineConfig&;

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
        Pipeline(VkDevice device, VkRenderPass renderPass, const PipelineConfig &config);
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

    inline auto PipelineConfig::withTopology(VkPrimitiveTopology topology) -> PipelineConfig&
    {
        this->topology_ = topology;
        return *this;
    }

    inline auto PipelineConfig::withPolygonMode(VkPolygonMode mode) -> PipelineConfig&
    {
        rasterStateInfo_.polygonMode = mode;
        return *this;
    }

    inline auto PipelineConfig::withDescriptorSetLayout(VkDescriptorSetLayout layout) -> PipelineConfig&
    {
        descSetLayouts_.push_back(layout);
        return *this;
    }

    inline auto PipelineConfig::withFrontFace(VkFrontFace frontFace) -> PipelineConfig&
    {
        rasterStateInfo_.frontFace = frontFace;
        return *this;
    }

    inline auto PipelineConfig::withCullMode(VkCullModeFlags cullFlags) -> PipelineConfig&
    {
        rasterStateInfo_.cullMode = cullFlags;
        return *this;
    }
}
