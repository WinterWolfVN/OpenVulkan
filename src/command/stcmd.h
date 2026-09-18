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
    int32_t isActive;
    std::vector<std::function<void()>> commands;
};
typedef VkCommandBuffer_T* VkCommandBuffer;

struct VkCommandBufferBeginInfo {
    int32_t sType;
    const void* pNext;
    uint32_t flags;
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
    uint32_t flags;
    int32_t waitSemaphoreInfoCount;
    int32_t commandBufferInfoCount;
    int32_t signalSemaphoreInfoCount;
};

struct VkImageSubresourceRange {
    int32_t aspectMask;
    int32_t baseMipLevel;
    int32_t levelCount;
    int32_t baseArrayLayer;
    int32_t layerCount;
};

struct VkImageMemoryBarrier {
    const void* pNext;
    VkImage image;
    uint32_t sType;
    uint32_t srcAccessMask;
    uint32_t dstAccessMask;
    int32_t oldLayout;
    int32_t newLayout;
    uint32_t srcQueueFamilyIndex;
    uint32_t dstQueueFamilyIndex;
    VkImageSubresourceRange subresourceRange;
};

struct VkImageMemoryBarrier2 {
    const void* pNext;
    VkImage image;
    uint64_t srcStageMask;
    uint64_t srcAccessMask;
    uint64_t dstStageMask;
    uint64_t dstAccessMask;
    int32_t sType;
    int32_t oldLayout;
    int32_t newLayout;
    uint32_t srcQueueFamilyIndex;
    uint32_t dstQueueFamilyIndex;
    VkImageSubresourceRange subresourceRange;
};

struct VkMemoryBarrier {
    const void* pNext;
    uint32_t sType;
    uint32_t srcAccessMask;
    uint32_t dstAccessMask;
};

struct VkMemoryBarrier2 {
    const void* pNext;
    uint64_t srcStageMask;
    uint64_t srcAccessMask;
    uint64_t dstStageMask;
    uint64_t dstAccessMask;
    int32_t sType;
};

struct VkBufferMemoryBarrier {
    const void* pNext;
    VkBuffer buffer;
    uint64_t offset;
    uint64_t size;
    uint32_t sType;
    uint32_t srcAccessMask;
    uint32_t dstAccessMask;
    uint32_t srcQueueFamilyIndex;
    uint32_t dstQueueFamilyIndex;
};

struct VkBufferMemoryBarrier2 {
    const void* pNext;
    VkBuffer buffer;
    int64_t offset;
    int64_t size;
    uint64_t srcStageMask;
    uint64_t srcAccessMask;
    uint64_t dstStageMask;
    uint64_t dstAccessMask;
    int32_t sType;
    uint32_t srcQueueFamilyIndex;
    uint32_t dstQueueFamilyIndex;
};

struct VkDependencyInfo {
    const void* pNext;
    const VkMemoryBarrier2* pMemoryBarriers;
    const VkBufferMemoryBarrier2* pBufferMemoryBarriers;
    const VkImageMemoryBarrier2* pImageMemoryBarriers;
    int32_t sType;
    uint32_t dependencyFlags;
    uint32_t memoryBarrierCount;
    uint32_t bufferMemoryBarrierCount;
    uint32_t imageMemoryBarrierCount;
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
