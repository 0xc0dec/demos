/**
 * Copyright (c) Aleksey Fedotov
 * MIT licence
 */

#include "VulkanPipeline.h"

static auto createShaderStageInfo(bool vertex, VkShaderModule shader, const char *entryPoint) -> VkPipelineShaderStageCreateInfo
{
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.stage = vertex ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
    info.module = shader;
    info.pName = entryPoint;
    info.pSpecializationInfo = nullptr;
    return info;
}

vk::Pipeline::Pipeline(VkDevice device, VkRenderPass renderPass, const PipelineConfig &config)
{
    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.pNext = nullptr;
    layoutInfo.flags = 0;
    layoutInfo.setLayoutCount = config.descSetLayouts_.size();
    layoutInfo.pSetLayouts = config.descSetLayouts_.data();
    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = nullptr;

    layout_ = Resource<VkPipelineLayout>{device, vkDestroyPipelineLayout};
    vk::ensure(vkCreatePipelineLayout(device, &layoutInfo, nullptr, layout_.cleanRef()));

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.pNext = nullptr;
    multisampleState.flags = 0;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.sampleShadingEnable = false;
    multisampleState.minSampleShading = 0;
    multisampleState.pSampleMask = nullptr;
    multisampleState.alphaToCoverageEnable = false;
    multisampleState.alphaToOneEnable = false;

    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.pNext = nullptr;
    colorBlendState.flags = 0;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_COPY;
    colorBlendState.attachmentCount = config.colorBlendAttachmentStates_.size();
    colorBlendState.pAttachments = config.colorBlendAttachmentStates_.data();
    colorBlendState.blendConstants[0] = 0;
    colorBlendState.blendConstants[1] = 0;
    colorBlendState.blendConstants[2] = 0;
    colorBlendState.blendConstants[3] = 0;

    const auto vertexShaderStageInfo = createShaderStageInfo(true, config.vs_, "main");
    const auto fragmentShaderStageInfo = createShaderStageInfo(false, config.fs_, "main");

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageStates{vertexShaderStageInfo, fragmentShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputState{};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.pNext = nullptr;
    vertexInputState.flags = 0;
    vertexInputState.vertexBindingDescriptionCount = config.vertexBindings_.size();
    vertexInputState.pVertexBindingDescriptions = config.vertexBindings_.data();
    vertexInputState.vertexAttributeDescriptionCount = config.vertexAttrs_.size();
    vertexInputState.pVertexAttributeDescriptions = config.vertexAttrs_.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.pNext = nullptr;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = config.topology_;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.flags = 0;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.flags = 0;
    pipelineInfo.stageCount = shaderStageStates.size();
    pipelineInfo.pStages = shaderStageStates.data();
    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &config.rasterStateInfo_;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &config.depthStencilStateInfo_;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = layout_;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    pipeline_ = Resource<VkPipeline>{device, vkDestroyPipeline};
    vk::ensure(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline_.cleanRef()));
}

vk::PipelineConfig::PipelineConfig(VkShaderModule vertexShader, VkShaderModule fragmentShader) : vs_(vertexShader),
                                                                                                 fs_(fragmentShader),
                                                                                                 rasterStateInfo_{},
                                                                                                 depthStencilStateInfo_{}
{
    rasterStateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterStateInfo_.pNext = nullptr;
    rasterStateInfo_.flags = 0;
    rasterStateInfo_.depthClampEnable = false;
    rasterStateInfo_.rasterizerDiscardEnable = false;
    rasterStateInfo_.polygonMode = VK_POLYGON_MODE_FILL;
    rasterStateInfo_.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterStateInfo_.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterStateInfo_.depthBiasEnable = false;
    rasterStateInfo_.depthBiasClamp = 0;
    rasterStateInfo_.depthBiasConstantFactor = 0;
    rasterStateInfo_.depthBiasSlopeFactor = 0;
    rasterStateInfo_.lineWidth = 1;

    depthStencilStateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateInfo_.flags = 0;
    depthStencilStateInfo_.depthTestEnable = true;
    depthStencilStateInfo_.depthWriteEnable = true;
    depthStencilStateInfo_.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilStateInfo_.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depthStencilStateInfo_.front = depthStencilStateInfo_.back;
}

auto vk::PipelineConfig::withColorBlendAttachmentCount(uint32_t count) -> PipelineConfig &
{
    // TODO More sophisticated once we start using blend state

    VkPipelineColorBlendAttachmentState tpl{};
    tpl.blendEnable = VK_FALSE;
    tpl.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    tpl.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    tpl.colorBlendOp = VK_BLEND_OP_ADD;
    tpl.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    tpl.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    tpl.alphaBlendOp = VK_BLEND_OP_ADD;
    tpl.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    colorBlendAttachmentStates_.assign(count, tpl);

    return *this;
}

auto vk::PipelineConfig::withVertexAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset) -> PipelineConfig &
{
    VkVertexInputAttributeDescription desc{};
    desc.location = location;
    desc.binding = binding;
    desc.format = format;
    desc.offset = offset;
    vertexAttrs_.push_back(desc);
    return *this;
}

auto vk::PipelineConfig::withVertexBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate) -> PipelineConfig &
{
    VkVertexInputBindingDescription desc{};
    desc.binding = binding;
    desc.stride = stride;
    desc.inputRate = inputRate;
    vertexBindings_.push_back(desc);
    return *this;
}

auto vk::PipelineConfig::withDepthTest(bool write, bool test) -> PipelineConfig &
{
    depthStencilStateInfo_.depthWriteEnable = write;
    depthStencilStateInfo_.depthTestEnable = test;
    return *this;
}

auto vk::PipelineConfig::withBlend(bool enabled,
                                   VkBlendFactor srcColorFactor, VkBlendFactor dstColorFactor,
                                   VkBlendFactor srcAlphaFactor, VkBlendFactor dstAlphaFactor) -> PipelineConfig &
{
    // TODO Better
    for (auto &state : colorBlendAttachmentStates_)
    {
        state.blendEnable = enabled ? VK_TRUE : VK_FALSE;
        state.srcColorBlendFactor = srcColorFactor;
        state.dstColorBlendFactor = dstColorFactor;
        state.srcAlphaBlendFactor = srcAlphaFactor;
        state.dstAlphaBlendFactor = dstAlphaFactor;
    }
    return *this;
}
