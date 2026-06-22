#pragma once
#include "stmng.h" 
#include <cstdint>
#include <vector>
#include <functional>

// [draw.cpp] 
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
    VkDescriptorBinding bindings[16];
    int32_t bindingCount;
    int32_t padding;
};
typedef struct VkDescriptorSet_T* VkDescriptorSet;

struct VkViewport {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
};
typedef struct VkViewport VkViewport;

struct VkExtent2D {
    int32_t width;
    int32_t height;
};
typedef struct VkExtent2D VkExtent2D;

struct VkOffset2D {
    int32_t x;
    int32_t y;
};
typedef struct VkOffset2D VkOffset2D;

struct VkRect2D {
    VkOffset2D offset;
    VkExtent2D extent;
};
typedef struct VkRect2D VkRect2D;

struct VkCommandBufferBeginInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    const void* pInheritanceInfo;
};
typedef struct VkCommandBufferBeginInfo VkCommandBufferBeginInfo;

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
typedef struct VkSubmitInfo VkSubmitInfo;

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
typedef struct VkClearDepthStencilValue VkClearDepthStencilValue;

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
typedef struct VkRenderPassBeginInfo VkRenderPassBeginInfo;

// [cmd_texture.cpp] 
struct VkOffset3D {
    int32_t x;
    int32_t y;
    int32_t z;
};
typedef struct VkOffset3D VkOffset3D;

struct VkImageSubresourceLayers {
    int32_t aspectMask;
    int32_t mipLevel;
    int32_t baseArrayLayer;
    int32_t layerCount;
};
typedef struct VkImageSubresourceLayers VkImageSubresourceLayers;

struct VkBufferImageCopy {
    int64_t bufferOffset;
    int32_t bufferRowLength;
    int32_t bufferImageHeight;
    struct VkImageSubresourceLayers imageSubresource;
    struct VkOffset3D imageOffset;
    struct VkExtent3D imageExtent;
};
typedef struct VkBufferImageCopy VkBufferImageCopy;

struct VkImageBlit {
    struct VkImageSubresourceLayers srcSubresource;
    struct VkOffset3D srcOffsets[2];
    struct VkImageSubresourceLayers dstSubresource;
    struct VkOffset3D dstOffsets[2];
};
typedef struct VkImageBlit VkImageBlit;

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
typedef struct VkImageMemoryBarrier VkImageMemoryBarrier; 
