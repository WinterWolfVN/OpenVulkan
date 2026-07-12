#include "../struct/stcmd.h"
#include <GLES3/gl31.h>
#include <cstdint>

extern "C" {

void vkCmdDraw(VkCommandBuffer commandBuffer, int32_t vertexCount, int32_t instanceCount, int32_t firstVertex, int32_t firstInstance) {
    if (!commandBuffer) return;    
    int32_t topo = commandBuffer->currentTopology;    
    commandBuffer->commands.push_back([=]() {
        if (instanceCount > 1) {
            glDrawArraysInstanced(static_cast<GLenum>(topo), firstVertex, vertexCount, instanceCount);
        } else {
            glDrawArrays(static_cast<GLenum>(topo), firstVertex, vertexCount);
        }
    });
}

void vkCmdDrawIndexed(VkCommandBuffer commandBuffer, int32_t indexCount, int32_t instanceCount, int32_t firstIndex, int32_t vertexOffset, int32_t firstInstance) {
    if (!commandBuffer) return;    
    int32_t topo = commandBuffer->currentTopology;
    int32_t iType = commandBuffer->currentIndexType;
    int64_t iOffset = commandBuffer->currentIndexOffset;    
    commandBuffer->commands.push_back([=]() {
        GLenum type = (iType == 0) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
        int64_t indexSize = (type == GL_UNSIGNED_SHORT) ? 2 : 4;
        int64_t finalOffset = iOffset + (firstIndex * indexSize);        
        if (instanceCount > 1) {
            glDrawElementsInstanced(static_cast<GLenum>(topo), indexCount, type, reinterpret_cast<const void*>(static_cast<uintptr_t>(finalOffset)), instanceCount);
        } else {
            glDrawElements(static_cast<GLenum>(topo), indexCount, type, reinterpret_cast<const void*>(static_cast<uintptr_t>(finalOffset)));
        }
    });
}

void vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, int64_t offset, int32_t drawCount, int32_t stride) {
    if (!commandBuffer || !buffer) return;    
    int32_t glBuf = buffer->buffer;
    int32_t topo = commandBuffer->currentTopology;    
    commandBuffer->commands.push_back([=]() {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, static_cast<GLuint>(glBuf));        
        for (int32_t i = 0; i < drawCount; ++i) {
            int64_t currentOffset = offset + (i * stride);
            glDrawArraysIndirect(static_cast<GLenum>(topo), reinterpret_cast<const void*>(static_cast<uintptr_t>(currentOffset)));
        }        
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    });
}

void vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, int64_t offset, int32_t drawCount, int32_t stride) {
    if (!commandBuffer || !buffer) return;    
    int32_t glBuf = buffer->buffer;
    int32_t topo = commandBuffer->currentTopology;
    int32_t iType = commandBuffer->currentIndexType;    
    commandBuffer->commands.push_back([=]() {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, static_cast<GLuint>(glBuf));
        GLenum type = (iType == 0) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;        
        for (int32_t i = 0; i < drawCount; ++i) {
            int64_t currentOffset = offset + (i * stride);
            glDrawElementsIndirect(static_cast<GLenum>(topo), type, reinterpret_cast<const void*>(static_cast<uintptr_t>(currentOffset)));
        }        
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    });
}
                
}
