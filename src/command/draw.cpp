#include "../struct/stcmd.h"
#include "../struct/stshader.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <vector>
#include <utility>

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

void vkCmdDispatch(VkCommandBuffer commandBuffer, int32_t groupCountX, int32_t groupCountY, int32_t groupCountZ) {
    if (!commandBuffer) return;    
    commandBuffer->commands.push_back([=]() {
        glDispatchCompute(static_cast<GLuint>(groupCountX), 
                          static_cast<GLuint>(groupCountY), 
                          static_cast<GLuint>(groupCountZ));
    });
}

void vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, int32_t pipelineBindPoint, VkPipelineLayout layout, int32_t firstSet, int32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, int32_t dynamicOffsetCount, const int32_t* pDynamicOffsets) {
    if (!commandBuffer || !pDescriptorSets || descriptorSetCount <= 0) return;    
    std::vector<VkDescriptorBinding> activeBindings;
    int32_t currentDynamicIdx = 0;    
    for (int32_t i = 0; i < descriptorSetCount; ++i) {
        if (pDescriptorSets[i]) {
            for (int32_t k = 0; k < pDescriptorSets[i]->bindingCount; ++k) {
                VkDescriptorBinding b = pDescriptorSets[i]->bindings[k];                
                if (b.type == 8 || b.type == 9) {
                    if (pDynamicOffsets && currentDynamicIdx < dynamicOffsetCount) {
                        b.offset += pDynamicOffsets[currentDynamicIdx];
                        currentDynamicIdx++;
                    }
                }                
                activeBindings.push_back(b);
            }
        }
    }    
    commandBuffer->commands.push_back([bindings = std::move(activeBindings)]() {
        for (const auto& b : bindings) {
            if (b.type == 6 || b.type == 8) { 
                glBindBufferRange(GL_UNIFORM_BUFFER, static_cast<GLuint>(b.binding), static_cast<GLuint>(b.bufferId), static_cast<GLintptr>(b.offset), static_cast<GLsizeiptr>(b.size));
            } 
            else if (b.type == 7 || b.type == 9) { 
                glBindBufferRange(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(b.binding), static_cast<GLuint>(b.bufferId), static_cast<GLintptr>(b.offset), static_cast<GLsizeiptr>(b.size));
            }
            else if (b.type == 1) { 
                glActiveTexture(GL_TEXTURE0 + static_cast<GLuint>(b.binding));
                glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(b.textureId));
                if (b.samplerId != 0) {
                    glBindSampler(static_cast<GLuint>(b.binding), static_cast<GLuint>(b.samplerId));
                }
            }
            else if (b.type == 3) { 
                glBindImageTexture(static_cast<GLuint>(b.binding), static_cast<GLuint>(b.textureId), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
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

void vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, int32_t firstBinding, int32_t bindingCount, const VkBuffer* pBuffers, const int64_t* pOffsets) {
    if (!commandBuffer || !pBuffers || bindingCount <= 0) return;    
    std::vector<int32_t> glBuffers(bindingCount);
    for (int32_t i = 0; i < bindingCount; ++i) {
        glBuffers[i] = pBuffers[i] ? pBuffers[i]->buffer : 0;
    }    
    commandBuffer->commands.push_back([buffers = std::move(glBuffers)]() {
        for (int32_t buf : buffers) {
            if (buf != 0) {
                glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(buf));
            }
        }
    });
}
    
}
