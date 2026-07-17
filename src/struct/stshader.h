#pragma once
#include <cstdint>

#define MAX_CACHE_ENTRIES 256

struct VkShaderModule_T {
    char* code;
    int64_t codeSize;
};
typedef struct VkShaderModule_T* VkShaderModule;

struct VkShaderModuleCreateInfo {
    const void* pNext;
    const uint32_t* pCode;    
    int64_t codeSize;    
    int32_t sType;
    int32_t flags;
};

struct VkPipeline_T {
    VkPipelineLayout layout;    
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
    const void* pNext;
    VkShaderModule module;
    const char* pName;
    const void* pSpecializationInfo;    
    int32_t sType;
    int32_t flags;
    int32_t stage;
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
    const void* pNext;
    const VkVertexInputBindingDescription* pVertexBindingDescriptions;
    const VkVertexInputAttributeDescription* pVertexAttributeDescriptions;    
    int32_t sType;
    int32_t flags;
    int32_t vertexBindingDescriptionCount;
    int32_t vertexAttributeDescriptionCount;
};

struct VkPipelineInputAssemblyStateCreateInfo {
    const void* pNext;    
    int32_t sType;
    int32_t flags;
    int32_t topology;
    int32_t primitiveRestartEnable;
};

struct VkPipelineRasterizationStateCreateInfo {
    const void* pNext;
    int32_t sType;
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
    const void* pNext;    
    int32_t sType;
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
    const void* pNext;
    const VkPipelineColorBlendAttachmentState* pAttachments;    
    int32_t sType;
    int32_t flags;
    int32_t logicOpEnable;
    int32_t logicOp;
    int32_t attachmentCount;
    float blendConstants[4];
};

struct VkGraphicsPipelineCreateInfo {
    const void* pNext;
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
    VkPipeline basePipelineHandle;    
    int32_t sType;
    int32_t flags;
    int32_t stageCount;
    int32_t subpass;
    int32_t basePipelineIndex;
};

struct VkComputePipelineCreateInfo {
    const void* pNext;
    VkPipelineLayout layout;
    VkPipeline basePipelineHandle;   
    VkPipelineShaderStageCreateInfo stage;    
    int32_t sType;
    int32_t flags;
    int32_t basePipelineIndex;
};

struct VkPushConstantRange {
    int32_t stageFlags;
    int32_t offset;
    int32_t size;
};

struct VkPipelineLayoutCreateInfo {
    const void* pNext;
    const VkDescriptorSetLayout* pSetLayouts;
    const VkPushConstantRange* pPushConstantRanges;    
    int32_t sType;
    int32_t flags;
    int32_t setLayoutCount;
    int32_t pushConstantRangeCount;
};

struct VkPipelineLayout_T {
    int64_t layoutId;
    VkDescriptorSetLayout* setLayouts;
    VkPushConstantRange* pushConstantRanges;    
    int32_t setLayoutCount;
    int32_t pushConstantRangeCount;
};
typedef struct VkPipelineLayout_T* VkPipelineLayout;

struct VkPipelineCacheHeader {
    uint64_t headerSize;
    uint64_t headerVersion;
    uint64_t vendorID;
    uint64_t deviceID;
    uint32_t pipelineCacheUUID[4];
};

struct VkPipelineCacheEntry {
    uint64_t hash;
    uint32_t* binaryData;
    uint64_t binarySize;
};

struct VkPipelineCache_T {
    int64_t cacheId;
    VkPipelineCacheEntry entries[MAX_CACHE_ENTRIES];    
    int32_t entryCount;
    int32_t flags;
};
typedef struct VkPipelineCache_T* VkPipelineCache;

struct VkPipelineCacheCreateInfo {
    const void* pNext;
    const void* pInitialData;    
    int32_t sType;
    int32_t flags;
    uint64_t initialDataSize;
};
