#include <GLES3/gl31.h>
#include <stdlib.h>
#include <stdint.h>

typedef void* VkDevice;
typedef uint64_t VkBuffer;
typedef uint64_t VkDeviceMemory;
typedef uint64_t VkDeviceSize;
typedef uint64_t VkPipeline;
typedef uint64_t VkPipelineLayout;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

struct FakeMemory {
    void* cpuData;
    VkDeviceSize size;
    GLuint boundBufferID;
};

extern "C" {

// --- Memory & Buffers ---
VkResult vkCreateBuffer(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkBuffer* pBuffer) {
    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    *pBuffer = (VkBuffer)bufferID;
    return VK_SUCCESS;
}

void vkDestroyBuffer(VkDevice device, VkBuffer buffer, const void* pAllocator) {
    glDeleteBuffers(1, (GLuint*)&buffer);
}

VkResult vkAllocateMemory(VkDevice device, const void* pAllocateInfo, const void* pAllocator, VkDeviceMemory* pMemory) {
    VkDeviceSize size = *(VkDeviceSize*)((char*)pAllocateInfo + 16);
    FakeMemory* mem = (FakeMemory*)malloc(sizeof(FakeMemory));
    mem->cpuData = malloc(size);
    mem->size = size;
    mem->boundBufferID = 0;
    *pMemory = (VkDeviceMemory)mem;
    return VK_SUCCESS;
}

void vkFreeMemory(VkDevice device, VkDeviceMemory memory, const void* pAllocator) {
    if (memory) {
        FakeMemory* mem = (FakeMemory*)memory;
        free(mem->cpuData);
        free(mem);
    }
}

VkResult vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    ((FakeMemory*)memory)->boundBufferID = (GLuint)buffer;
    return VK_SUCCESS;
}

VkResult vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, uint32_t flags, void** ppData) {
    *ppData = (char*)((FakeMemory*)memory)->cpuData + offset;
    return VK_SUCCESS;
}

void vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    FakeMemory* mem = (FakeMemory*)memory;
    if (mem->boundBufferID != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, mem->boundBufferID);
        glBufferData(GL_ARRAY_BUFFER, mem->size, mem->cpuData, GL_STATIC_DRAW);
    }
}

// --- Pipeline ---
VkResult vkCreatePipelineLayout(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkPipelineLayout* pPipelineLayout) {
    if (pPipelineLayout) *pPipelineLayout = (VkPipelineLayout)1;
    return VK_SUCCESS;
}

void vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout layout, const void* pAllocator) {}

VkResult vkCreateGraphicsPipelines(VkDevice device, uint64_t pipelineCache, uint32_t createInfoCount, const void* pCreateInfos, const void* pAllocator, VkPipeline* pPipelines) {
    if (pPipelines) *pPipelines = (VkPipeline)1;
    return VK_SUCCESS;
}

void vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const void* pAllocator) {}

}
