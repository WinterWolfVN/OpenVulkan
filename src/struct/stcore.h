#pragma once
#include "stcmd.h" 
#include "stbuffer.h"
#include "stmng.h" 

// [buffer.cpp]
typedef struct VkDeviceMemory_T* VkDeviceMemory;
typedef struct VkBuffer_T* VkBuffer;

// [texture.cpp] 
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VkSampler_T* VkSampler;

typedef struct VkExtent3D VkExtent3D;
typedef struct VkComponentMapping VkComponentMapping;
typedef struct VkImageSubresourceRange VkImageSubresourceRange;
typedef struct VkImageCreateInfo VkImageCreateInfo;
typedef struct VkImageViewCreateInfo VkImageViewCreateInfo;
typedef struct VkSamplerCreateInfo VkSamplerCreateInfo;

// [draw.cpp] 
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkPipeline_T* VkPipeline;
typedef struct VkFramebuffer_T* VkFramebuffer;

typedef struct VkExtent2D VkExtent2D;
typedef struct VkOffset2D VkOffset2D;
typedef struct VkRect2D VkRect2D;
typedef struct VkViewport VkViewport;
typedef struct VkClearDepthStencilValue VkClearDepthStencilValue;
typedef struct VkCommandBufferBeginInfo VkCommandBufferBeginInfo;
typedef struct VkRenderPassBeginInfo VkRenderPassBeginInfo;
typedef struct VkSubmitInfo VkSubmitInfo;

// [cmd_texture.cpp] 
typedef struct VkOffset3D VkOffset3D;
typedef struct VkImageSubresourceLayers VkImageSubresourceLayers;
typedef struct VkBufferImageCopy VkBufferImageCopy;
typedef struct VkImageBlit VkImageBlit;
typedef struct VkImageMemoryBarrier VkImageMemoryBarrier;        

// [descriptor.cpp] 
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;
typedef struct VkDescriptorPool_T* VkDescriptorPool;
typedef struct VkDescriptorSet_T* VkDescriptorSet;

typedef struct VkDescriptorBinding VkDescriptorBinding;
typedef struct VkDescriptorBufferInfo VkDescriptorBufferInfo;
typedef struct VkDescriptorImageInfo VkDescriptorImageInfo;
typedef struct VkWriteDescriptorSet VkWriteDescriptorSet;
typedef struct VkDescriptorSetLayoutCreateInfo VkDescriptorSetLayoutCreateInfo;
typedef struct VkDescriptorPoolCreateInfo VkDescriptorPoolCreateInfo;
typedef struct VkDescriptorSetAllocateInfo VkDescriptorSetAllocateInfo;
