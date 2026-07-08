#pragma once
#include <cstdint>

#define MAX_BINDINGS 256

// [descriptor.cpp]
typedef enum VkDescriptorType {
    VK_DESCRIPTOR_TYPE_SAMPLER = 0,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
    VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
    VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK = 1000138000
} VkDescriptorType;

struct VkDescriptorSetLayoutBinding {
    const VkSampler* pImmutableSamplers;
    int32_t binding;
    int32_t descriptorType;
    int32_t descriptorCount;
    int32_t stageFlags;
};

struct VkDescriptorSetLayout_T {
    VkDescriptorSetLayoutBinding* bindings;
    int64_t layoutId;
    int32_t bindingCount;
};
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;

struct VkDescriptorPool_T {
    int64_t poolId;
    VkDescriptorSet* allocatedSetList;
    int32_t maxSets;
    int32_t allocatedSets;
    int32_t maxDesc[12];
    int32_t allocDesc[12];
};
typedef struct VkDescriptorPool_T* VkDescriptorPool;

struct VkDescriptorBinding {
    int64_t offset;
    int64_t size;
    int32_t binding;
    int32_t arrayElement;
    int32_t type;
    int32_t bufferId;
    int32_t textureId;
    int32_t samplerId;
    int32_t imageTarget;
    int32_t imageFormat;
    int32_t imageLayout;
    int32_t isImmutableSampler;
};

struct VkDescriptorSet_T {
    int64_t setId;
    VkDescriptorSetLayout layout;
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

struct VkCopyDescriptorSet {
    const void* pNext;
    VkDescriptorSet srcSet;
    VkDescriptorSet dstSet;
    int32_t sType;
    int32_t srcBinding;
    int32_t srcArrayElement;
    int32_t dstBinding;
    int32_t dstArrayElement;
    int32_t descriptorCount;
};

struct VkDescriptorSetLayoutCreateInfo {
    const void* pNext;
    const VkDescriptorSetLayoutBinding* pBindings;
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
