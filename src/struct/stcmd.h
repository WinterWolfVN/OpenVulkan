#pragma once
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
