#pragma once
#include <cstdint>
#include <vector>
#include <functional>

// [buffer.cpp] 
struct VkDeviceMemory_T {
    void* hostPtr;
    int64_t size;
    int32_t memoryTypeIndex;
    std::atomic<int32_t> mappedCount;
    int32_t isMapped;
};
typedef struct VkDeviceMemory_T* VkDeviceMemory;

struct VkBuffer_T {
    int64_t memoryOffset;
    int64_t size;
    int32_t buffer;    
    int32_t usage;
    struct VkDeviceMemory_T* memory;    
};
typedef struct VkBuffer_T* VkBuffer;

// [texture.cpp] 
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
typedef struct VkImage_T* VkImage;

struct VkImageView_T {
    VkImage image;
    int32_t viewType;
    int32_t format;
    int32_t baseMipLevel;
    int32_t levelCount;
    int32_t baseArrayLayer;
    int32_t layerCount;
};
typedef struct VkImageView_T* VkImageView;

struct VkSampler_T {
    int32_t sampler;
};
typedef struct VkSampler_T* VkSampler;

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
