#include <GLES3/gl31.h>
#include <vector>
#include <stdint.h>

struct VkDevice_T;
struct VkQueue_T;
struct VkFence_T;
struct VkSemaphore_T;
struct VkPipelineLayout_T;

struct DescriptorInfo {
    uint32_t binding;
    uint32_t descriptorType;
    uint32_t descriptorCount;
    uint32_t stageFlags;
};

struct DescriptorBinding {
    uint32_t binding;
    uint32_t type;

    GLuint gles_object;

    GLsizeiptr size;
};

struct VkCommandPool_T {
    uint32_t flags;
};

struct VkCommandBuffer_T {
    std::vector<uint8_t> stream;

    VkCommandPool_T* commandPool;

    bool recording = false;
};

struct VkPipelineLayout_T {
};

struct VkPipeline_T {
    GLuint gles_program = 0;

    VkPipelineLayout_T* layout = nullptr;
};

struct VkBuffer_T {
    GLuint gles_buffer = 0;

    uint64_t size = 0;
};

struct VkDescriptorSetLayout_T {
    std::vector<DescriptorInfo> bindings;
};

struct VkDescriptorPool_T {
    uint32_t maxSets = 0;

    uint32_t allocatedSets = 0;
};

struct VkDescriptorSet_T {

    std::vector<DescriptorBinding> bindings;

    GLuint gles_texture = 0;

    GLuint gles_ubo = 0;

    GLsizeiptr ubo_size = 0;
};

struct VkRenderPass_T {

    bool clear_color = true;

    bool clear_depth = true;

    bool clear_stencil = false;
};

struct VkFramebuffer_T {
    GLuint gles_fbo = 0;
};

struct VkQueue_T {
    uint32_t familyIndex = 0;
};

struct VkFence_T {
    bool signaled = false;
};

struct VkSemaphore_T {
};

struct VkDevice_T {
};
