#pragma once
#include <cstdint>

struct VkShaderModule_T {
    char* code;
    int64_t codeSize;
};
typedef struct VkShaderModule_T* VkShaderModule;

struct VkShaderModuleCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int64_t codeSize;
    const uint32_t* pCode;
};

struct VkPipeline_T {
    uint32_t program;
    int32_t bindPoint;
    int32_t topology;
    int32_t depthTestEnable;
    int32_t depthCompareOp;
    int32_t cullModeEnable;
    int32_t cullFace;
    int32_t frontFace;
    int32_t blendEnable;
    int32_t blendSrcFactor;
    int32_t blendDstFactor;
    int32_t blendEquation;
};
typedef struct VkPipeline_T* VkPipeline;

typedef struct VkPipelineCache_T* VkPipelineCache;

struct VkPipelineShaderStageCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t stage;
    VkShaderModule module;
    const char* pName;
    const void* pSpecializationInfo;
};

struct VkVertexInputBindingDescription {
    int32_t binding;
    int32_t stride;
    int32_t inputRate;
};

struct VkVertexInputAttributeDescription {
    int32_t location;
    int32_t binding;
    int32_t format;
    int32_t offset;
};

struct VkPipelineVertexInputStateCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t vertexBindingDescriptionCount;
    const VkVertexInputBindingDescription* pVertexBindingDescriptions;
    int32_t vertexAttributeDescriptionCount;
    const VkVertexInputAttributeDescription* pVertexAttributeDescriptions;
};

struct VkPipelineInputAssemblyStateCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t topology;
    int32_t primitiveRestartEnable;
};

struct VkPipelineRasterizationStateCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t depthClampEnable;
    int32_t rasterizerDiscardEnable;
    int32_t polygonMode;
    int32_t cullMode;
    int32_t frontFace;
    int32_t depthBiasEnable;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;
    float lineWidth;
};

struct VkPipelineDepthStencilStateCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t depthTestEnable;
    int32_t depthWriteEnable;
    int32_t depthCompareOp;
    int32_t depthBoundsTestEnable;
    int32_t stencilTestEnable;
    int32_t front[7];
    int32_t back[7];
    float minDepthBounds;
    float maxDepthBounds;
};

struct VkPipelineColorBlendAttachmentState {
    int32_t blendEnable;
    int32_t srcColorBlendFactor;
    int32_t dstColorBlendFactor;
    int32_t colorBlendOp;
    int32_t srcAlphaBlendFactor;
    int32_t dstAlphaBlendFactor;
    int32_t alphaBlendOp;
    int32_t colorWriteMask;
};

struct VkPipelineColorBlendStateCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t logicOpEnable;
    int32_t logicOp;
    int32_t attachmentCount;
    const VkPipelineColorBlendAttachmentState* pAttachments;
    float blendConstants[4];
};

struct VkGraphicsPipelineCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t stageCount;
    const VkPipelineShaderStageCreateInfo* pStages;
    const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
    const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
    const void* pTessellationState;
    const void* pViewportState;
    const VkPipelineRasterizationStateCreateInfo* pRasterizationState;
    const void* pMultisampleState;
    const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
    const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
    const void* pDynamicState;
    VkPipelineLayout layout;
    VkRenderPass renderPass;
    int32_t subpass;
    VkPipeline basePipelineHandle;
    int32_t basePipelineIndex;
};

struct VkComputePipelineCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    VkPipelineShaderStageCreateInfo stage;
    VkPipelineLayout layout;
    VkPipeline basePipelineHandle;
    int32_t basePipelineIndex;
};
