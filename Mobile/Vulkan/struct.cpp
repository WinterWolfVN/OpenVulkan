#include <GLES3/gl31.h>
#include <stdint.h>
#include <vector>

typedef int32_t VkResult;
typedef uint64_t VkDeviceSize;

#define VK_SUCCESS 0
#define VK_ERROR_OUT_OF_HOST_MEMORY -1
#define VK_ERROR_OUT_OF_DEVICE_MEMORY -2
#define VK_ERROR_INITIALIZATION_FAILED -3
#define VK_ERROR_DEVICE_LOST -4

enum VkIndexType {
    VK_INDEX_TYPE_UINT16 = 0,
    VK_INDEX_TYPE_UINT32 = 1
};

struct VkInstance_T;
struct VkPhysicalDevice_T;
struct VkDevice_T;
struct VkQueue_T;
struct VkFence_T;
struct VkSemaphore_T;

struct VkCommandPool_T;
struct VkCommandBuffer_T;
struct VkPipelineLayout_T;
struct VkPipeline_T;
struct VkBuffer_T;
struct VkDescriptorSetLayout_T;
struct VkDescriptorPool_T;
struct VkDescriptorSet_T;
struct VkRenderPass_T;
struct VkFramebuffer_T;

typedef VkInstance_T* VkInstance;
typedef VkPhysicalDevice_T* VkPhysicalDevice;
typedef VkDevice_T* VkDevice;
typedef VkQueue_T* VkQueue;
typedef VkFence_T* VkFence;
typedef VkSemaphore_T* VkSemaphore;

typedef VkCommandPool_T* VkCommandPool;
typedef VkCommandBuffer_T* VkCommandBuffer;
typedef VkPipelineLayout_T* VkPipelineLayout;
typedef VkPipeline_T* VkPipeline;
typedef VkBuffer_T* VkBuffer;
typedef VkDescriptorSetLayout_T* VkDescriptorSetLayout;
typedef VkDescriptorPool_T* VkDescriptorPool;
typedef VkDescriptorSet_T* VkDescriptorSet;
typedef VkRenderPass_T* VkRenderPass;
typedef VkFramebuffer_T* VkFramebuffer;

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
    uint32_t flags = 0;
};

struct VkCommandBuffer_T {
    std::vector<uint8_t> stream;
    VkCommandPool commandPool = nullptr;
    bool recording = false;
};

struct VkPipelineLayout_T {
};

struct VkPipeline_T {
    GLuint gles_program = 0;
    GLuint gles_pipeline = 0;
    VkPipelineLayout layout = nullptr;
};

struct VkBuffer_T {
    GLuint gles_buffer = 0;
    VkDeviceSize size = 0;
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
    uint32_t width = 0;
    uint32_t height = 0;
};

struct VkQueue_T {
    uint32_t familyIndex = 0;
};

struct VkFence_T {
    bool signaled = false;
};

struct VkSemaphore_T {
};

struct VkInstance_T {
};

struct VkPhysicalDevice_T {
};

struct VkDevice_T {
};
