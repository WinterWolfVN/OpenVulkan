#pragma once
#include <cstdint>

#define MAX_BINDINGS 32

// [descriptor.cpp]
struct VkDescriptorSetLayout_T {
    int32_t dummy;
};
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;

struct VkDescriptorPool_T {
    int64_t poolId;
    int32_t maxSets;
    int32_t allocatedSets;
    int32_t maxUniformBuffers;
    int32_t maxCombinedImages;
};
typedef struct VkDescriptorPool_T* VkDescriptorPool;

struct VkDescriptorBinding {
    int64_t offset;
    int64_t size;
    int32_t binding;
    int32_t type;
    int32_t bufferId;
    int32_t textureId;
    int32_t samplerId;
};

struct VkDescriptorSet_T {
    int64_t setId;
    VkDescriptorBinding bindings[MAX_BINDINGS];
    int32_t bindingCount;
};
typedef struct VkDescriptorSet_T* VkDescriptorSet;

struct VkDescriptorBufferInfo {
    VkBuffer buffer;
    int64_t offset;
    int64_t range;
};

struct VkDescriptorImageInfo {
    VkSampler sampler;
    VkImageView imageView;
    int32_t imageLayout;
};

struct VkWriteDescriptorSet {
    const void* pNext;
    VkDescriptorSet dstSet;
    const VkDescriptorImageInfo* pImageInfo;
    const VkDescriptorBufferInfo* pBufferInfo;
    const void* pTexelBufferView;
    int32_t sType;
    int32_t dstBinding;
    int32_t dstArrayElement;
    int32_t descriptorCount;
    int32_t descriptorType;
};

struct VkDescriptorSetLayoutCreateInfo {
    const void* pNext;
    const void* pBindings;
    int32_t sType;
    int32_t flags;
    int32_t bindingCount;
};

struct VkDescriptorPoolSize {
    int32_t type;
    int32_t descriptorCount;
};

struct VkDescriptorPoolCreateInfo {
    const void* pNext;
    const VkDescriptorPoolSize* pPoolSizes;
    int32_t sType;
    int32_t flags;
    int32_t maxSets;
    int32_t poolSizeCount;
};

struct VkDescriptorSetAllocateInfo {
    const void* pNext;
    VkDescriptorPool descriptorPool;
    const VkDescriptorSetLayout* pSetLayouts;
    int32_t sType;
    int32_t descriptorSetCount;
};
