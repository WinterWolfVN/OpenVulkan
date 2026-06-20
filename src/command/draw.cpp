#include "../fpa/struct_core.h"
#include <GLES3/gl31.h>
#include <cstdint>

int32_t vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) {
    if (!commandBuffer) return -3;
    
    commandBuffer->currentTopology = 0;
    commandBuffer->currentIndexType = 0;
    commandBuffer->currentIndexOffset = 0;
    
    return 0;
}

void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
    if (!commandBuffer) return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, int32_t firstBinding, int32_t bindingCount, const VkBuffer* pBuffers, const int64_t* pOffsets) {
    if (!commandBuffer || !pBuffers || !pOffsets) return;
    
    for (int32_t i = 0; i < bindingCount; ++i) {
        if (pBuffers[i]) {
            glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(pBuffers[i]->buffer));
        }
    }
}

void vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, int64_t offset, int32_t indexType) {
    if (!commandBuffer || !buffer) return;
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(buffer->buffer));
    commandBuffer->currentIndexType = indexType;
    commandBuffer->currentIndexOffset = offset;
}

void vkCmdSetViewport(VkCommandBuffer commandBuffer, int32_t firstViewport, int32_t viewportCount, const VkViewport* pViewports) {
    if (!commandBuffer || !pViewports || viewportCount <= 0) return;
    
    glViewport(static_cast<GLint>(pViewports[0].x),
               static_cast<GLint>(pViewports[0].y),
               static_cast<GLsizei>(pViewports[0].width),
               static_cast<GLsizei>(pViewports[0].height));
               
    glDepthRangef(pViewports[0].minDepth, pViewports[0].maxDepth);
}

void vkCmdSetScissor(VkCommandBuffer commandBuffer, int32_t firstScissor, int32_t scissorCount, const VkRect2D* pScissors) {
    if (!commandBuffer || !pScissors || scissorCount <= 0) return;
    
    glScissor(static_cast<GLint>(pScissors[0].offset.x),
              static_cast<GLint>(pScissors[0].offset.y),
              static_cast<GLsizei>(pScissors[0].extent.width),
              static_cast<GLsizei>(pScissors[0].extent.height));
}

void vkCmdBindPipeline(VkCommandBuffer commandBuffer, int32_t pipelineBindPoint, VkPipeline pipeline) {
    if (!commandBuffer || !pipeline) return;
    
    glUseProgram(static_cast<GLuint>(pipeline->program));
    commandBuffer->currentTopology = pipeline->topology;
    
    if (pipeline->depthTestEnable) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(static_cast<GLenum>(pipeline->depthCompareOp));
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    
    if (pipeline->cullModeEnable) {
        glEnable(GL_CULL_FACE);
        glCullFace(static_cast<GLenum>(pipeline->cullFace));
        glFrontFace(static_cast<GLenum>(pipeline->frontFace));
    } else {
        glDisable(GL_CULL_FACE);
    }
    
    if (pipeline->blendEnable) {
        glEnable(GL_BLEND);
        glBlendFunc(static_cast<GLenum>(pipeline->blendSrcFactor), static_cast<GLenum>(pipeline->blendDstFactor));
        glBlendEquation(static_cast<GLenum>(pipeline->blendEquation));
    } else {
        glDisable(GL_BLEND);
    }
}

void vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, int32_t pipelineBindPoint, VkPipelineLayout layout, int32_t firstSet, int32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, int32_t dynamicOffsetCount, const int32_t* pDynamicOffsets) {
    if (!commandBuffer || !pDescriptorSets) return;
    
    for (int32_t i = 0; i < descriptorSetCount; ++i) {
        if (pDescriptorSets[i]) {
            glBindBufferRange(GL_UNIFORM_BUFFER,
                              static_cast<GLuint>(pDescriptorSets[i]->binding),
                              static_cast<GLuint>(pDescriptorSets[i]->uniformBuffer),
                              static_cast<GLintptr>(pDescriptorSets[i]->offset),
                              static_cast<GLsizeiptr>(pDescriptorSets[i]->size));
        }
    }
}

void vkCmdDraw(VkCommandBuffer commandBuffer, int32_t vertexCount, int32_t instanceCount, int32_t firstVertex, int32_t firstInstance) {
    if (!commandBuffer) return;
    
    GLenum topology = static_cast<GLenum>(commandBuffer->currentTopology);
    
    if (instanceCount > 1) {
        glDrawArraysInstanced(topology, firstVertex, vertexCount, instanceCount);
    } else {
        glDrawArrays(topology, firstVertex, vertexCount);
    }
}

void vkCmdDrawIndexed(VkCommandBuffer commandBuffer, int32_t indexCount, int32_t instanceCount, int32_t firstIndex, int32_t vertexOffset, int32_t firstInstance) {
    if (!commandBuffer) return;
    
    GLenum topology = static_cast<GLenum>(commandBuffer->currentTopology);
    GLenum type = (commandBuffer->currentIndexType == 0) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    int64_t indexSize = (type == GL_UNSIGNED_SHORT) ? 2 : 4;
    int64_t finalOffset = commandBuffer->currentIndexOffset + (firstIndex * indexSize);
    
    if (instanceCount > 1) {
        glDrawElementsInstanced(topology, indexCount, type, reinterpret_cast<const void*>(static_cast<uintptr_t>(finalOffset)), instanceCount);
    } else {
        glDrawElements(topology, indexCount, type, reinterpret_cast<const void*>(static_cast<uintptr_t>(finalOffset)));
    }
}

void vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, int64_t offset, int32_t drawCount, int32_t stride) {
    if (!commandBuffer || !buffer) return;
    
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, static_cast<GLuint>(buffer->buffer));
    GLenum topology = static_cast<GLenum>(commandBuffer->currentTopology);
    
    for (int32_t i = 0; i < drawCount; ++i) {
        int64_t currentOffset = offset + (i * stride);
        glDrawArraysIndirect(topology, reinterpret_cast<const void*>(static_cast<uintptr_t>(currentOffset)));
    }
    
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, int64_t offset, int32_t drawCount, int32_t stride) {
    if (!commandBuffer || !buffer) return;
    
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, static_cast<GLuint>(buffer->buffer));
    GLenum topology = static_cast<GLenum>(commandBuffer->currentTopology);
    GLenum type = (commandBuffer->currentIndexType == 0) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    
    for (int32_t i = 0; i < drawCount; ++i) {
        int64_t currentOffset = offset + (i * stride);
        glDrawElementsIndirect(topology, type, reinterpret_cast<const void*>(static_cast<uintptr_t>(currentOffset)));
    }
    
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

int32_t vkQueueSubmit(VkQueue queue, int32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    glFlush();
    return 0;
}
