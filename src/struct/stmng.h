/#pragma once
#include <cstdint>

//[descriptor.cpp] 
struct VkDescriptorSetLayout_T {
    int32_t dummy;
};

struct VkDescriptorPool_T {
    int32_t dummy;
};

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

struct VkDescriptorSetLayoutCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t bindingCount;
    const void* pBindings;
};

struct VkDescriptorPoolCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int32_t maxSets;
    int32_t poolSizeCount;
    const void* pPoolSizes;
};

struct VkDescriptorSetAllocateInfo {
    int32_t sType;
    const void* pNext;
    VkDescriptorPool descriptorPool;
    int32_t descriptorSetCount;
    const VkDescriptorSetLayout* pSetLayouts;
};

struct VkDescriptorBinding {
    int32_t binding;
    int32_t type;
    int32_t bufferId;
    int64_t offset;
    int64_t size;
    int32_t textureId;
    int32_t samplerId;
};

