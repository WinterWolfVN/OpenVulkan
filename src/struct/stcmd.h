#pragma once
#include "stmng.h" 
#include <cstdint>
#include <vector>
#include <functional>

#define MAX_BARRIERS 64

struct VkCommandBuffer_T {
    int64_t currentIndexOffset;
    int64_t cmdId;
    int32_t currentTopology;
    int32_t currentIndexType;
    int32_t renderTargetWidth;
    int32_t renderTargetHeight;        
    VkMemoryBarrier2 memoryBarriers[MAX_BARRIERS];
    VkBufferMemoryBarrier2 bufferBarriers[MAX_BARRIERS];
    VkImageMemoryBarrier2 imageBarriers[MAX_BARRIERS];    
    int32_t memoryBarrierCount;
    int32_t bufferBarrierCount;
    int32_t imageBarrierCount;    
    int32_t isActive;
    std::vector<std::function<void()>> commands;
};
typedef struct VkCommandBuffer_T* VkCommandBuffer;

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

struct VkSubmitInfo2 {
    const void* pNext;
    const VkSemaphoreSubmitInfo* pWaitSemaphoreInfos;
    const VkCommandBufferSubmitInfo* pCommandBufferInfos;
    const VkSemaphoreSubmitInfo* pSignalSemaphoreInfos;
    int32_t sType;
    int32_t flags;
    int32_t waitSemaphoreInfoCount;
    int32_t commandBufferInfoCount;
    int32_t signalSemaphoreInfoCount;
};

struct VkFramebuffer_T {
    GLuint fbo;
    int32_t width;
    int32_t height;
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

struct VkImageMemoryBarrier2 {
    const void* pNext;
    VkImage image;
    int64_t srcStageMask;
    int64_t srcAccessMask;
    int64_t dstStageMask;
    int64_t dstAccessMask;
    int32_t sType;
    int32_t oldLayout;
    int32_t newLayout;
    int32_t srcQueueFamilyIndex;
    int32_t dstQueueFamilyIndex;
};

struct VkMemoryBarrier {
    const void* pNext;
    int32_t sType;
    int32_t srcAccessMask;
    int32_t dstAccessMask;
};

struct VkMemoryBarrier2 {
    const void* pNext;
    int64_t srcStageMask;
    int64_t srcAccessMask;
    int64_t dstStageMask;
    int64_t dstAccessMask;
    int32_t sType;
};

struct VkBufferMemoryBarrier {
    const void* pNext;
    VkBuffer buffer;
    int64_t offset;
    int64_t size;
    int32_t sType;
    int32_t srcAccessMask;
    int32_t dstAccessMask;
    int32_t srcQueueFamilyIndex;
    int32_t dstQueueFamilyIndex;
};

struct VkBufferMemoryBarrier2 {
    const void* pNext;
    VkBuffer buffer;
    int64_t offset;
    int64_t size;
    int64_t srcStageMask;
    int64_t srcAccessMask;
    int64_t dstStageMask;
    int64_t dstAccessMask;
    int32_t sType;
    int32_t srcQueueFamilyIndex;
    int32_t dstQueueFamilyIndex;
};

struct VkDependencyInfo {
    const void* pNext;
    const VkMemoryBarrier2* pMemoryBarriers;
    const VkBufferMemoryBarrier2* pBufferMemoryBarriers;
    const VkImageMemoryBarrier2* pImageMemoryBarriers;
    int32_t sType;
    int32_t dependencyFlags;
    int32_t memoryBarrierCount;
    int32_t bufferMemoryBarrierCount;
    int32_t imageMemoryBarrierCount;
};

struct VkRenderingAttachmentInfo {
    const void* pNext;
    VkImageView imageView;
    VkImageView resolveImageView;
    int32_t sType;
    int32_t imageLayout;
    int32_t resolveMode;
    int32_t resolveImageLayout;
    int32_t loadOp;
    int32_t storeOp;
    float clearValue[4];
};

struct VkRenderingInfo {
    const void* pNext;
    const VkRenderingAttachmentInfo* pColorAttachments;
    const VkRenderingAttachmentInfo* pDepthAttachment;
    const VkRenderingAttachmentInfo* pStencilAttachment;
    int32_t sType;
    int32_t flags;
    int32_t renderAreaX;
    int32_t renderAreaY;
    int32_t renderAreaWidth;
    int32_t renderAreaHeight;
    int32_t layerCount;
    int32_t viewMask;
    int32_t colorAttachmentCount;
};

struct VkCommandBufferSubmitInfo {
    const void* pNext;
    VkCommandBuffer commandBuffer;
    int32_t sType;
    int32_t deviceMask;
};

struct VkSemaphoreSubmitInfo {
    const void* pNext;
    VkSemaphore semaphore;
    int64_t value;
    int64_t stageMask;
    int32_t sType;
    int32_t deviceIndex;
};
