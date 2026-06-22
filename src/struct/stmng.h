#pragma once
#include <cstdint>

// [descriptor.cpp] 
struct VkDescriptorSetLayout_T {
    int32_t dummy;
};
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;

struct VkDescriptorPool_T {
    int32_t dummy;
};
typedef struct VkDescriptorPool_T* VkDescriptorPool;

struct VkDescriptorBufferInfo {
    VkBuffer buffer;
    int64_t offset;
    int64_t range;
};
typedef struct VkDescriptorBufferInfo VkDescriptorBufferInfo;

struct VkDescriptorImageInfo {
    VkSampler sampler;
    VkImageView imageView;
    int32_t imageLayout;
};
typedef struct VkDescriptorImageInfo VkDescriptorImageInfo;

struct VkWriteDescriptorSet {
    int32_t sType;
    const void* pNext;
    VkDescriptorSet dstSet;
    int32_t dstBinding;
    int32_t dstArrayElement;
    int32_t descriptorCount;
    int32_t descriptorType;
    const VkDescriptorImageInfo* pImageInfo;
    const VkDescriptorBufferInfo* pBufferInfo;
    const void* pTexelBufferView;
};
typedef struct VkWriteDescriptorSet VkWriteDescriptorSet;

struct VkDescriptorSetLayoutCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t bindingCount;
    const void* pBindings;
};
typedef struct VkDescriptorSetLayoutCreateInfo VkDescriptorSetLayoutCreateInfo;

struct VkDescriptorPoolCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t maxSets;
    int32_t poolSizeCount;
    const void* pPoolSizes;
};
typedef struct VkDescriptorPoolCreateInfo VkDescriptorPoolCreateInfo;

struct VkDescriptorSetAllocateInfo {
    int32_t sType;
    const void* pNext;
    VkDescriptorPool descriptorPool;
    int32_t descriptorSetCount;
    const VkDescriptorSetLayout* pSetLayouts;
};
typedef struct VkDescriptorSetAllocateInfo VkDescriptorSetAllocateInfo;

struct VkDescriptorBinding {
    int32_t binding;
    int32_t type;
    int32_t bufferId;
    int64_t offset;
    int64_t size;
    int32_t textureId;
    int32_t samplerId;
};
typedef struct VkDescriptorBinding VkDescriptorBinding;
