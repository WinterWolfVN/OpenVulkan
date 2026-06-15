#pragma once
#include <GLES3/gl31.h>
#include <vector>
#include <stdint.h>

struct DescriptorInfo {
    uint32_t binding;
    uint32_t descriptorType;
    uint32_t descriptorCount;
    uint32_t stageFlags;
};

struct VkCommandPool_T { 
    uint32_t flags; 
};

struct VkCommandBuffer_T {
    std::vector<uint8_t> stream;
    struct VkCommandPool_T* commandPool;
};

struct VkPipeline_T { 
    GLuint gles_program; 
};

struct VkBuffer_T { 
    GLuint gles_buffer; 
    uint64_t size; 
};

struct VkDescriptorSet_T {
    GLuint gles_texture;
    GLuint gles_ubo;
    GLsizeiptr ubo_size;
};

struct VkDescriptorSetLayout_T {
    std::vector<DescriptorInfo> bindings;
};

struct VkDescriptorPool_T {
    uint32_t maxSets;
};

struct VkRenderPass_T { 
    bool clear_color; 
};

struct VkFramebuffer_T { 
    GLuint gles_fbo; 
};

struct VkQueue_T {};
