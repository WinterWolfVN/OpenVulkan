#include <GLES3/gl31.h>
#include <stdint.h>

typedef void* VkDevice;
typedef void* VkCommandPool;
typedef void* VkCommandBuffer;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

struct FakeCmdAllocInfo {
    uint32_t sType;
    const void* pNext;
    VkCommandPool commandPool;
    uint32_t level;
    uint32_t commandBufferCount;
};

extern "C" {

// --- Command Pool & Buffers ---
VkResult vkCreateCommandPool(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkCommandPool* pCommandPool) {
    if (pCommandPool) *pCommandPool = (VkCommandPool)1;
    return VK_SUCCESS;
}

void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const void* pAllocator) {}

VkResult vkAllocateCommandBuffers(VkDevice device, const void* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {
    if (!pAllocateInfo || !pCommandBuffers) return VK_SUCCESS;
    const FakeCmdAllocInfo* info = (const FakeCmdAllocInfo*)pAllocateInfo;
    for (uint32_t i = 0; i < info->commandBufferCount; ++i) {
        pCommandBuffers[i] = (VkCommandBuffer)(uintptr_t)(i + 1);
    }
    return VK_SUCCESS;
}

void vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {}

VkResult vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const void* pBeginInfo) { return VK_SUCCESS; }
VkResult vkEndCommandBuffer(VkCommandBuffer commandBuffer) { return VK_SUCCESS; }
VkResult vkResetCommandBuffer(VkCommandBuffer commandBuffer, uint32_t flags) { return VK_SUCCESS; }
VkResult vkResetCommandPool(VkDevice device, VkCommandPool commandPool, uint32_t flags) { return VK_SUCCESS; }

// --- Command Execution & Drawing ---
void vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const void* pRenderPassBegin, uint32_t contents) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) { glFlush(); }

void vkCmdBindPipeline(VkCommandBuffer cmd, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {   
    glBindProgramPipeline((GLuint)pipeline);
}

void vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t first, uint32_t count, const uint64_t* pBuffers, const uint64_t* pOffsets) {
    if (count > 0) glBindBuffer(GL_ARRAY_BUFFER, (GLuint)pBuffers[0]);
}

void vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, uint64_t buffer, uint64_t offset, uint32_t indexType) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)buffer);
}

void vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, uint32_t bindPoint, uint32_t layout, uint32_t firstSet, uint32_t setCount, const uint64_t* pSets, uint32_t dynCount, const uint32_t* pDynOffsets) {}
void vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const void* pViewports) {}
void vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const void* pScissors) {}

void vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
}

void vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)(uintptr_t)firstIndex);
}

void vkCmdDrawIndirect(VkCommandBuffer commandBuffer, uint64_t buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) {
    glDrawArraysIndirect(GL_TRIANGLES, (const void*)(uintptr_t)offset);
}

void vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, uint64_t buffer, uint64_t offset, uint32_t drawCount, uint32_t stride) {
    glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)(uintptr_t)offset);
}

void vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    for (uint32_t i = 0; i < submitCount; ++i) {        
        CommandBuffer& cmd = *(CommandBuffer*)pSubmits[i].pCommandBuffers[0];                
        for (auto& command : cmd.commands) {
            switch(command.type) {
                case CMD_BIND_PIPELINE:
                    glBindProgramPipeline((GLuint)command.pipeline);
                    break;
                case CMD_BIND_VERTEX_BUFFERS:
                    glBindVertexBuffer(0, (GLuint)command.buffer, command.offset, command.stride);
                    break;
                case CMD_DRAW:
                    glDrawArrays(command.mode, command.first, command.count);
                    break;
            }
        }
    }
}

}
