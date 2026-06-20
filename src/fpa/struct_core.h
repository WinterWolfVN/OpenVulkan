#pragma once
#include <atomic>
#include <cstdint>

//[buffer.cpp] 
struct VkDeviceMemory_T {
    void* hostPtr;
    int64_t size;
    int32_t memoryTypeIndex;
    std::atomic<int32_t> mappedCount;
    int32_t isMapped;
};

struct VkBuffer_T {
    int64_t size;
    int64_t memoryOffset;
    int32_t buffer;    
    int32_t usage;
    struct VkDeviceMemory_T* memory;    
};

//[draw.cpp] 
struct VkCommandBuffer_T {
    int32_t currentTopology;
    int32_t currentIndexType;
    int64_t currentIndexOffset;
};
typedef struct VkCommandBuffer_T* VkCommandBuffer;

struct VkPipeline_T {
    int32_t program;
    int32_t topology;
};
typedef struct VkPipeline_T* VkPipeline;

typedef struct VkQueue_T* VkQueue;
typedef struct VkPipelineLayout_T* VkPipelineLayout;
typedef struct VkDescriptorSet_T* VkDescriptorSet;
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

struct VkSubmitInfo {
    VkStructureType sType;
    const void* pNext;
    int32_t waitSemaphoreCount;
    const void* pWaitSemaphores;
    const int32_t* pWaitDstStageMask;
    int32_t commandBufferCount;
    const VkCommandBuffer* pCommandBuffers;
    int32_t signalSemaphoreCount;
    const void* pSignalSemaphores;
};
