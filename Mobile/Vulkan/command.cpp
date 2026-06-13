#include <GLES3/gl31.h>
#include <stdint.h>
#include <vector>
#include <map>

typedef void* VkDevice;
typedef void* VkQueue;
typedef uint64_t VkCommandPool;
typedef uint64_t VkCommandBuffer;
typedef uint64_t VkBuffer;
typedef uint64_t VkDescriptorSet;
typedef void* VkFence;
typedef uint32_t VkResult;
typedef uint64_t VkDeviceSize;
typedef uint32_t VkIndexType;
typedef uint32_t VkPipelineBindPoint;
typedef uint64_t VkPipeline;

#define VK_SUCCESS 0
#define VK_INDEX_TYPE_UINT16 0

enum CommandType {
    CMD_BIND_PIPELINE,
    CMD_BIND_VERTEX_BUFFERS,
    CMD_BIND_INDEX_BUFFER,
    CMD_BIND_DESCRIPTOR_SETS,
    CMD_DRAW,
    CMD_DRAW_INDEXED,
    CMD_DRAW_INDIRECT,
    CMD_DRAW_INDEXED_INDIRECT,
    CMD_BEGIN_RENDER_PASS,
    CMD_END_RENDER_PASS
};

struct DescriptorResource {
    GLenum type;
    GLuint id;
};

struct VkDescriptorSet_T {
    std::map<uint32_t, DescriptorResource> resources;
};

struct BufferContext {
    GLuint glBuffer;
};

struct Command {
    CommandType type;
    uintptr_t pipeline;
    uintptr_t buffer;
    GLintptr offset;
    GLsizei stride;
    VkDescriptorSet descriptorSet;
    GLenum mode;
    GLint first;
    GLsizei count;
    GLenum indexType;
    const void* indices;
    uint32_t instanceCount;
};

struct CommandBuffer {
    std::vector<Command> commands;
    bool isRecording;
};

struct VkCommandPool_T {
    std::vector<CommandBuffer*> allocatedBuffers;
};

struct VkSubmitInfo {
    uint32_t sType;
    const void* pNext;
    uint32_t waitSemaphoreCount;
    const void* pWaitSemaphores;
    const uint32_t* pWaitDstStageMask;
    uint32_t commandBufferCount;
    const VkCommandBuffer* pCommandBuffers;
    uint32_t signalSemaphoreCount;
    const void* pSignalSemaphores;
};

extern "C" {

VkResult vkCreateCommandPool(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkCommandPool* pCommandPool) {
    *pCommandPool = (VkCommandPool)(uintptr_t)new VkCommandPool_T();
    return VK_SUCCESS;
}

void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const void* pAllocator) {
    auto* pool = (VkCommandPool_T*)(uintptr_t)commandPool;
    if (pool) {
        for (auto* buf : pool->allocatedBuffers) {
            delete buf;
        }
        delete pool;
    }
}

VkResult vkAllocateCommandBuffers(VkDevice device, const void* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {
    auto* raw = (const uint32_t*)pAllocateInfo;
    VkCommandPool poolHandle = *(const VkCommandPool*)(raw + 4); 
    uint32_t count = *(raw + 7);
    auto* pool = (VkCommandPool_T*)(uintptr_t)poolHandle;
    for (uint32_t i = 0; i < count; ++i) {
        auto* buf = new CommandBuffer();
        buf->isRecording = false;
        pool->allocatedBuffers.push_back(buf);
        pCommandBuffers[i] = (VkCommandBuffer)(uintptr_t)buf;
    }
    return VK_SUCCESS;
}

void vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    auto* pool = (VkCommandPool_T*)(uintptr_t)commandPool;
    if (!pool) return;
    for (uint32_t i = 0; i < commandBufferCount; ++i) {
        auto* buf = (CommandBuffer*)(uintptr_t)pCommandBuffers[i];
        for (auto it = pool->allocatedBuffers.begin(); it != pool->allocatedBuffers.end(); ++it) {
            if (*it == buf) {
                delete buf;
                pool->allocatedBuffers.erase(it);
                break;
            }
        }
    }
}

VkResult vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const void* pBeginInfo) {
    auto& cmd = *(CommandBuffer*)commandBuffer;
    cmd.commands.clear();
    cmd.isRecording = true;
    return VK_SUCCESS;
}

VkResult vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    auto& cmd = *(CommandBuffer*)commandBuffer;
    cmd.isRecording = false;
    return VK_SUCCESS;
}

VkResult vkResetCommandBuffer(VkCommandBuffer commandBuffer, uint32_t flags) { return VK_SUCCESS; }
VkResult vkResetCommandPool(VkDevice device, VkCommandPool commandPool, uint32_t flags) { return VK_SUCCESS; }

void vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const void* pRenderPassBegin, uint32_t contents) {
    auto& cmd = *(CommandBuffer*)commandBuffer;
    Command c;
    c.type = CMD_BEGIN_RENDER_PASS;
    cmd.commands.push_back(c);
}

void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) { 
    auto& cmd = *(CommandBuffer*)commandBuffer;
    Command c;
    c.type = CMD_END_RENDER_PASS;
    cmd.commands.push_back(c);
}

void vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t first, uint32_t count, const uint64_t* pBuffers, const uint64_t* pOffsets) {
    if (count > 0) {
        auto& cmd = *(CommandBuffer*)commandBuffer;
        Command c;
        c.type = CMD_BIND_VERTEX_BUFFERS;
        c.buffer = (uintptr_t)pBuffers[0];
        c.offset = pOffsets ? (GLintptr)pOffsets[0] : 0;
        c.stride = 32;
        cmd.commands.push_back(c);
    }
}

void vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) {
    auto& cmd = *(CommandBuffer*)commandBuffer;
    Command c;
    c.type = CMD_BIND_INDEX_BUFFER;
    c.buffer = (uintptr_t)buffer;
    c.offset = (GLintptr)offset;
    c.indexType = (indexType == VK_INDEX_TYPE_UINT16) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    cmd.commands.push_back(c);
}

void vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const void* pViewports) {}
void vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const void* pScissors) {}

void vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    auto& cmd = *(CommandBuffer*)commandBuffer;
    Command c;
    c.type = CMD_DRAW;
    c.mode = GL_TRIANGLES;
    c.first = (GLint)firstVertex;
    c.count = (GLsizei)vertexCount;
    cmd.commands.push_back(c);
}

void vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    auto& cmd = *(CommandBuffer*)commandBuffer;
    Command c;
    c.type = CMD_DRAW_INDEXED;
    c.mode = GL_TRIANGLES;
    c.count = (GLsizei)indexCount;
    c.indices = (const void*)(uintptr_t)firstIndex;
    c.instanceCount = instanceCount;
    cmd.commands.push_back(c);
}

void vkCmdDrawIndirect(VkCommandBuffer commandBuffer, uint64_t buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) {
    auto& cmd = *(CommandBuffer*)commandBuffer;
    Command c;
    c.type = CMD_DRAW_INDIRECT;
    c.offset = (GLintptr)offset;
    cmd.commands.push_back(c);
}

void vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, uint64_t buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) {
    auto& cmd = *(CommandBuffer*)commandBuffer;
    Command c;
    c.type = CMD_DRAW_INDEXED_INDIRECT;
    c.offset = (GLintptr)offset;
    cmd.commands.push_back(c);
}

void vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {   
    auto& cmd = *(CommandBuffer*)commandBuffer;
    Command c;
    c.type = CMD_BIND_PIPELINE;
    c.pipeline = (uintptr_t)pipeline;
    cmd.commands.push_back(c);
}

void vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);    
    for (uint32_t i = 0; i < submitCount; ++i) {        
        CommandBuffer& cmd = *(CommandBuffer*)pSubmits[i].pCommandBuffers[0];        
        VkDescriptorSet_T* activeDescriptorSet = nullptr;        
        GLenum activeIndexType = GL_UNSIGNED_SHORT;
        GLintptr activeIndexOffset = 0;
        for (auto& command : cmd.commands) {
            switch(command.type) {
                case CMD_BEGIN_RENDER_PASS:
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    break;
                case CMD_END_RENDER_PASS:
                    glFlush();
                    break;
                case CMD_BIND_PIPELINE:
                    glBindProgramPipeline((GLuint)command.pipeline);
                    break;                    
                case CMD_BIND_VERTEX_BUFFERS:
                    glBindVertexBuffer(0, (GLuint)command.buffer, command.offset, command.stride);
                    break;
                case CMD_BIND_INDEX_BUFFER: {
                    BufferContext* ctx = (BufferContext*)(uintptr_t)command.buffer;
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->glBuffer);
                    activeIndexType = command.indexType;
                    activeIndexOffset = command.offset;
                    break;
                }
                case CMD_BIND_DESCRIPTOR_SETS:
                    activeDescriptorSet = (VkDescriptorSet_T*)(uintptr_t)command.descriptorSet;
                    break;                    
                case CMD_DRAW:
                    if (activeDescriptorSet) {
                        for (auto const& [binding, res] : activeDescriptorSet->resources) {
                            if (res.type == GL_UNIFORM_BUFFER) {
                                glBindBufferBase(GL_UNIFORM_BUFFER, binding, res.id);
                            } else if (res.type == GL_SAMPLER_2D) {
                                glActiveTexture(GL_TEXTURE0 + binding);
                                glBindTexture(GL_TEXTURE_2D, res.id);
                            }
                        }
                    }
                    glDrawArrays(command.mode, command.first, command.count);
                    break;
                case CMD_DRAW_INDEXED:
                    if (activeDescriptorSet) {
                        for (auto const& [binding, res] : activeDescriptorSet->resources) {
                            if (res.type == GL_UNIFORM_BUFFER) {
                                glBindBufferBase(GL_UNIFORM_BUFFER, binding, res.id);
                            } else if (res.type == GL_SAMPLER_2D) {
                                glActiveTexture(GL_TEXTURE0 + binding);
                                glBindTexture(GL_TEXTURE_2D, res.id);
                            }
                        }
                    }
                    glDrawElementsInstanced(
                        command.mode, 
                        command.count, 
                        activeIndexType, 
                        (void*)(activeIndexOffset + ((uintptr_t)command.indices * (activeIndexType == GL_UNSIGNED_SHORT ? 2 : 4))), 
                        command.instanceCount
                    );
                    break;
                case CMD_DRAW_INDIRECT:
                    glDrawArraysIndirect(GL_TRIANGLES, (const void*)command.offset);
                    break;
                case CMD_DRAW_INDEXED_INDIRECT:
                    glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)command.offset);
                    break;
            }
        }
    }
    glFlush();
}

}
