#pragma once
#include <atomic>
#include <cstdint>

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
