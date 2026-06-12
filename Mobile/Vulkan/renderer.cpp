#include <GLES3/gl31.h>
#include <stdint.h>

typedef void* VkCommandBuffer;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

extern "C" {

void vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {    
    glEnableVertexAttribArray(0);        
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);    
    if (instanceCount > 1) {
        glDrawArraysInstanced(GL_TRIANGLES, firstVertex, vertexCount, instanceCount);
    } else {
        glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
    }
        
    glDisableVertexAttribArray(0);
}

void vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    GLenum type = GL_UNSIGNED_INT; 
    
    if (instanceCount > 1) {
        glDrawElementsInstanced(GL_TRIANGLES, indexCount, type, (void*)(uintptr_t)(firstIndex * 4), instanceCount);
    } else {
        glDrawElements(GL_TRIANGLES, indexCount, type, (void*)(uintptr_t)(firstIndex * 4));
    }
    
    glDisableVertexAttribArray(0);
}

void vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const float* pViewports) {
    if (pViewports) {        
        glViewport((GLint)pViewports[0], (GLint)pViewports[1], (GLsizei)pViewports[2], (GLsizei)pViewports[3]);
    }
}

void vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const int32_t* pScissors) {
    if (pScissors) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(pScissors[0], pScissors[1], pScissors[2], pScissors[3]);
    }
}

}
