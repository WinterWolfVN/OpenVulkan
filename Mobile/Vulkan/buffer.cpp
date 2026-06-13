#include <GLES3/gl31.h>
#include <stdlib.h>
#include <string.h>

typedef void* VkDevice;
typedef uint64_t VkBuffer;
typedef uint64_t VkDeviceMemory;
typedef uint64_t VkDeviceSize;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

struct FakeMemory {
    void* cpuData;
    VkDeviceSize size;
    GLuint boundBufferID;
};

extern "C" {

VkResult vkCreateBuffer(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkBuffer* pBuffer) {
    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    *pBuffer = (VkBuffer)bufferID;
    return VK_SUCCESS;
}

void vkDestroyBuffer(VkDevice device, VkBuffer buffer, const void* pAllocator) {
    GLuint bufferID = (GLuint)buffer;
    glDeleteBuffers(1, &bufferID);
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
    FakeMemory* mem = (FakeMemory*)memory;
    mem->boundBufferID = (GLuint)buffer;
    return VK_SUCCESS;
}

VkResult vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, uint32_t flags, void** ppData) {
    FakeMemory* mem = (FakeMemory*)memory;
    *ppData = (char*)mem->cpuData + offset;
    return VK_SUCCESS;
}

void vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    FakeMemory* mem = (FakeMemory*)memory;
    if (mem->boundBufferID != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, mem->boundBufferID);
        glBufferData(GL_ARRAY_BUFFER, mem->size, mem->cpuData, GL_STATIC_DRAW);
    }
}

void vkCmdBindVertexBuffers(void* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const uint64_t* pOffsets) {
    if (bindingCount > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, (GLuint)pBuffers[0]);
    }
}

void vkCmdBindIndexBuffer(void* commandBuffer, VkBuffer buffer, uint64_t offset, uint32_t indexType) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)buffer);
}

void CreateAndBindVAO(GLuint* vaoID) {
    glGenVertexArrays(1, vaoID);
    glBindVertexArray(*vaoID);
}

}
