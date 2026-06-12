#include <GLES3/gl31.h>
#include <stdlib.h>

typedef void* VkDevice;
typedef uint64_t VkBuffer;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

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
