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
    int32_t buffer;
    int64_t size;
    int32_t usage;
    struct VkDeviceMemory_T* memory;
    int64_t memoryOffset;
};

//[draw.cpp] 
struct VkCommandBuffer_T {
    int32_t currentTopology;
    int32_t currentIndexType;
    int64_t currentIndexOffset;
    std::vector<std::function<void()>> commands;
};
typedef struct VkCommandBuffer_T* VkCommandBuffer;

struct VkPipeline_T {
    int32_t program;
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
    int32_t uniformBuffer;
    int64_t offset;
    int64_t size;
    int32_t binding;
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
