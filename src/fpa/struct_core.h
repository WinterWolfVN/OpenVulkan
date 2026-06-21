#pragma once
#include <atomic>
#include <cstdint>
#include <vector>
#include <functional>

//[buffer.cpp] 
struct VkDeviceMemory_T {
    void* hostPtr;
    int64_t size;
    int32_t memoryTypeIndex;
    std::atomic<int32_t> mappedCount;
    int32_t isMapped;
};

struct VkBuffer_T {
    int64_t memoryOffset;
    int64_t size;
    int32_t buffer;    
    int32_t usage;
    struct VkDeviceMemory_T* memory;    
};

//[texture.cpp] 
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VkSampler_T* VkSampler;

struct VkImage_T {
    int32_t texture;
    int32_t imageType;
    int32_t format;
    int32_t width;
    int32_t height;
    int32_t depth;
    int32_t mipLevels;
    int32_t arrayLayers;
};

struct VkImageView_T {
    VkImage image;
    int32_t viewType;
    int32_t format;
};

struct VkSampler_T {
    int32_t sampler;
};

struct VkExtent3D {
    int32_t width;
    int32_t height;
    int32_t depth;
};

struct VkImageCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t imageType;
    int32_t format;
    struct VkExtent3D extent;
    int32_t mipLevels;
    int32_t arrayLayers;
    int32_t samples;
    int32_t tiling;
    int32_t usage;
    int32_t sharingMode;
    int32_t queueFamilyIndexCount;
    const int32_t* pQueueFamilyIndices;
    int32_t initialLayout;
};

struct VkComponentMapping {
    int32_t r;
    int32_t g;
    int32_t b;
    int32_t a;
};

struct VkImageSubresourceRange {
    int32_t aspectMask;
    int32_t baseMipLevel;
    int32_t levelCount;
    int32_t baseArrayLayer;
    int32_t layerCount;
};

struct VkImageViewCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    VkImage image;
    int32_t viewType;
    int32_t format;
    struct VkComponentMapping components;
    struct VkImageSubresourceRange subresourceRange;
};

struct VkSamplerCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t magFilter;
    int32_t minFilter;
    int32_t mipmapMode;
    int32_t addressModeU;
    int32_t addressModeV;
    int32_t addressModeW;
    float mipLodBias;
    int32_t anisotropyEnable;
    float maxAnisotropy;
    int32_t compareEnable;
    int32_t compareOp;
    float minLod;
    float maxLod;
    int32_t borderColor;
    int32_t unnormalizedCoordinates;
};

//[command_texture.cpp] 
struct VkOffset3D {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct VkImageSubresourceLayers {
    int32_t aspectMask;
    int32_t mipLevel;
    int32_t baseArrayLayer;
    int32_t layerCount;
};

struct VkBufferImageCopy {
    int64_t bufferOffset;
    int32_t bufferRowLength;
    int32_t bufferImageHeight;
    struct VkImageSubresourceLayers imageSubresource;
    struct VkOffset3D imageOffset;
    struct VkExtent3D imageExtent;
};

struct VkImageBlit {
    struct VkImageSubresourceLayers srcSubresource;
    struct VkOffset3D srcOffsets[2];
    struct VkImageSubresourceLayers dstSubresource;
    struct VkOffset3D dstOffsets[2];
};

struct VkImageMemoryBarrier {
    const void* pNext;
    VkImage image;
    int32_t sType;
    int32_t srcAccessMask;
    int32_t dstAccessMask;
    int32_t oldLayout;
    int32_t newLayout;
    int32_t srcQueueFamilyIndex;
    int32_t dstQueueFamilyIndex;
    struct VkImageSubresourceRange subresourceRange;
};

//[draw.cpp] 
struct VkCommandBuffer_T {
    int64_t currentIndexOffset;
    int32_t currentTopology;
    int32_t currentIndexType;    
    std::vector<std::function<void()>> commands;
};
typedef struct VkCommandBuffer_T* VkCommandBuffer;

struct VkPipeline_T {
    int32_t program;
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

struct VkDescriptorSet_T {
    int64_t offset;
    int64_t size;
    VkImage image;
    VkSampler sampler;
    int32_t uniformBuffer;
    int32_t binding;
    int32_t descriptorType;
};
typedef struct VkDescriptorSet_T* VkDescriptorSet;

typedef struct VkQueue_T* VkQueue;
typedef struct VkPipelineLayout_T* VkPipelineLayout;
typedef struct VkFence_T* VkFence;

struct VkViewport {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
};

struct VkExtent2D {
    int32_t width;
    int32_t height;
};

struct VkOffset2D {
    int32_t x;
    int32_t y;
};

struct VkRect2D {
    VkOffset2D offset;
    VkExtent2D extent;
};

struct VkCommandBufferBeginInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    const void* pInheritanceInfo;
};

struct VkSubmitInfo {
    int32_t sType;
    const void* pNext;
    int32_t waitSemaphoreCount;
    const void* pWaitSemaphores;
    const int32_t* pWaitDstStageMask;
    int32_t commandBufferCount;
    const VkCommandBuffer* pCommandBuffers;
    int32_t signalSemaphoreCount;
    const void* pSignalSemaphores;
};

typedef struct VkRenderPass_T* VkRenderPass;

struct VkFramebuffer_T {
    int32_t fbo;
};
typedef struct VkFramebuffer_T* VkFramebuffer;

union VkClearColorValue {
    float float32[4];
    int32_t int32[4];
};

struct VkClearDepthStencilValue {
    float depth;
    int32_t stencil;
};

union VkClearValue {
    union VkClearColorValue color;
    struct VkClearDepthStencilValue depthStencil;
};

struct VkRenderPassBeginInfo {
    int32_t sType;
    const void* pNext;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
    struct VkRect2D renderArea;
    int32_t clearValueCount;
    const union VkClearValue* pClearValues;
};
