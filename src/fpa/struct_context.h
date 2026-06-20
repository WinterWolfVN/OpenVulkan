#pragma once
#include <EGL/egl.h>
#include <cstdint>
#include <mutex>
#include <atomic>

//[context.cpp] 
struct VkInstance_T {
    EGLDisplay display;
};
typedef VkInstance_T* VkInstance;

struct VkPhysicalDevice_T {
    char deviceName[256];
    EGLDisplay display;
};
typedef VkPhysicalDevice_T* VkPhysicalDevice;

struct VkDevice_T {
    EGLContext context;
    EGLSurface surface;
    EGLDisplay display;
};
typedef VkDevice_T* VkDevice;

struct VkMemoryType {
    int32_t propertyFlags; 
    int32_t heapIndex;
};

struct VkMemoryHeap {
    int64_t size;
    int32_t flags;
};

struct VkPhysicalDeviceMemoryProperties {
    int32_t memoryTypeCount;
    VkMemoryType memoryTypes[4];
    int32_t memoryHeapCount;
    VkMemoryHeap memoryHeaps[2];
};

struct VkPhysicalDeviceProperties {
    int32_t apiVersion;
    int32_t driverVersion;
    int32_t vendorID;
    int32_t deviceID;
    char deviceName[256];
};
