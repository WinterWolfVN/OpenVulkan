#include "../fpa/struct_core.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <vector>

int32_t vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) {
    if (!commandBuffer) return -3;
    
    commandBuffer->currentTopology = 0;
    commandBuffer->currentIndexType = 0;
    commandBuffer->currentIndexOffset = 0;
    commandBuffer->commands.clear(); 
    
    return 0;
}

int32_t vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    if (!commandBuffer) return -3;
    
    return 0;
}

void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
    if (!commandBuffer) return;
    
    commandBuffer->commands.push_back([]() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
}

void vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, int32_t firstBinding, int32_t bindingCount, const VkBuffer* pBuffers, const int64_t* pOffsets) {
    if (!commandBuffer || !pBuffers || !pOffsets) return;
    
    std::vector<int32_t> glBuffers(bindingCount);
    for (int32_t i = 0; i < bindingCount; ++i) {
        glBuffers[i] = pBuffers[i] ? pBuffers[i]->buffer : 0;
    }
    
    commandBuffer->commands.push_back([glBuffers]() {
        for (int32_t buf : glBuffers) {
            if (buf != 0) {
                glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(buf));
            }
        }
    });
}

void vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, int64_t offset, int32_t indexType) {
    if (!commandBuffer || !buffer) return;
    
    int32_t glBuf = buffer->buffer;
    commandBuffer->currentIndexType = indexType;
    commandBuffer->currentIndexOffset = offset;
    
    commandBuffer->commands.push_back([glBuf]() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(glBuf));
    });
}

void vkCmdSetViewport(VkCommandBuffer commandBuffer, int32_t firstViewport, int32_t viewportCount, const VkViewport* pViewports) {
    if (!commandBuffer || !pViewports || viewportCount <= 0) return;
    
    VkViewport vp = pViewports[0];
    
    commandBuffer->commands.push_back([vp]() {
        glViewport(static_cast<GLint>(vp.x),
                   static_cast<GLint>(vp.y),
                   static_cast<GLsizei>(vp.width),
                   static_cast<GLsizei>(vp.height));
        glDepthRangef(vp.minDepth, vp.maxDepth);
    });
}

void vkCmdSetScissor(VkCommandBuffer commandBuffer, int32_t firstScissor, int32_t scissorCount, const VkRect2D* pScissors) {
    if (!commandBuffer || !pScissors || scissorCount <= 0) return;
    
    VkRect2D sc = pScissors[0];
    
    commandBuffer->commands.push_back([sc]() {
        glScissor(static_cast<GLint>(sc.offset.x),
                  static_cast<GLint>(sc.offset.y),
                  static_cast<GLsizei>(sc.extent.width),
                  static_cast<GLsizei>(sc.extent.height));
    });
}

void vkCmdBindPipeline(VkCommandBuffer commandBuffer, int32_t pipelineBindPoint, VkPipeline pipeline) {
    if (!commandBuffer || !pipeline) return;
    
    int32_t prog = pipeline->program;
    int32_t isCompute = (pipelineBindPoint == 1);    
    int32_t topo = pipeline->topology;
    int32_t dTest = pipeline->depthTestEnable;
    int32_t dOp = pipeline->depthCompareOp;
    int32_t cEnable = pipeline->cullModeEnable;
    int32_t cFace = pipeline->cullFace;
    int32_t fFace = pipeline->frontFace;
    int32_t bEnable = pipeline->blendEnable;
    int32_t bSrc = pipeline->blendSrcFactor;
    int32_t bDst = pipeline->blendDstFactor;
    int32_t bEq = pipeline->blendEquation;
    
    commandBuffer->commands.push_back([=]() {
        glUseProgram(static_cast<GLuint>(prog));        
        if (!isCompute) {
            if (dTest) {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(static_cast<GLenum>(dOp));
            } else {
                glDisable(GL_DEPTH_TEST);
            }
            
            if (cEnable) {
                glEnable(GL_CULL_FACE);
                glCullFace(static_cast<GLenum>(cFace));
                glFrontFace(static_cast<GLenum>(fFace));
            } else {
                glDisable(GL_CULL_FACE);
            }
            
            if (bEnable) {
                glEnable(GL_BLEND);
                glBlendFunc(static_cast<GLenum>(bSrc), static_cast<GLenum>(bDst));
                glBlendEquation(static_cast<GLenum>(bEq));
            } else {
                glDisable(GL_BLEND);
            }
        }
    });

    if (!isCompute) {
        commandBuffer->currentTopology = topo;
    }
}

void vkCmdDispatch(VkCommandBuffer commandBuffer, int32_t groupCountX, int32_t groupCountY, int32_t groupCountZ) {
    if (!commandBuffer) return;
    
    commandBuffer->commands.push_back([=]() {
        glDispatchCompute(static_cast<GLuint>(groupCountX), 
                          static_cast<GLuint>(groupCountY), 
                          static_cast<GLuint>(groupCountZ));
    });
}

void vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, int32_t pipelineBindPoint, VkPipelineLayout layout, int32_t firstSet, int32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, int32_t dynamicOffsetCount, const int32_t* pDynamicOffsets) {
    if (!commandBuffer || !pDescriptorSets) return;
    
    std::vector<VkDescriptorSet_T> sets(descriptorSetCount);
    for (int32_t i = 0; i < descriptorSetCount; ++i) {
        if (pDescriptorSets[i]) sets[i] = *(pDescriptorSets[i]);
    }
    
    commandBuffer->commands.push_back([sets]() {
        for (const auto& set : sets) {
            glBindBufferRange(GL_UNIFORM_BUFFER,
                              static_cast<GLuint>(set.binding),
                              static_cast<GLuint>(set.uniformBuffer),
                              static_cast<GLintptr>(set.offset),
                              static_cast<GLsizeiptr>(set.size));
        }
    });
}

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

int32_t vkQueueSubmit(VkQueue queue, int32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    if (!pSubmits) return -3;
    
    for (int32_t i = 0; i < submitCount; ++i) {
        for (int32_t j = 0; j < pSubmits[i].commandBufferCount; ++j) {
            VkCommandBuffer cmd = pSubmits[i].pCommandBuffers[j];
            if (cmd) {
                for (const auto& command : cmd->commands) {
                    command();
                }
            }
        }
    }
    
    glFlush();
    return 0;
}
