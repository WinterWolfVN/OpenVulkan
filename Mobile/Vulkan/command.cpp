#include <GLES3/gl31.h>
#include <vector>
#include <stdint.h>
#include <string.h>

enum class Opcode : uint8_t {
    BindPipeline, BindVertexBuffers, BindIndexBuffer, BindDescriptorSets, Draw, DrawIndexed, DrawIndirect, DrawIndexedIndirect, SetViewport, SetScissor, BeginRenderPass, EndRenderPass
};

VkResult vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) {
    *pCommandPool = new VkCommandPool_T{ pCreateInfo->flags };
    return VK_SUCCESS;
}

void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) {
    delete commandPool;
}

VkResult vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) {
    return VK_SUCCESS;
}

VkResult vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {
    for (uint32_t i = 0; i < pAllocateInfo->commandBufferCount; ++i) {
        pCommandBuffers[i] = new VkCommandBuffer_T{ std::vector<uint8_t>(), pAllocateInfo->commandPool };
    }
    return VK_SUCCESS;
}

void vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    for (uint32_t i = 0; i < commandBufferCount; ++i) delete pCommandBuffers[i];
}

VkResult vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) {
    if (commandBuffer) commandBuffer->stream.clear();
    return VK_SUCCESS;
}

VkResult vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) {
    if (commandBuffer) commandBuffer->stream.clear();
    return VK_SUCCESS;
}

void vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {
    commandBuffer->stream.push_back((uint8_t)Opcode::BindPipeline);
    commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&pipeline, (uint8_t*)&pipeline + sizeof(void*));
}

void vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) {
    if (pBuffers && bindingCount > 0) {
        commandBuffer->stream.push_back((uint8_t)Opcode::BindVertexBuffers);
        commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&pBuffers[0], (uint8_t*)&pBuffers[0] + sizeof(void*));
    }
}

void vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) {
    commandBuffer->stream.push_back((uint8_t)Opcode::BindIndexBuffer);
    commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&buffer, (uint8_t*)&buffer + sizeof(void*));
}

void vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) {
    uintptr_t dummy = 0;
    commandBuffer->stream.push_back((uint8_t)Opcode::BindDescriptorSets);
    commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&dummy, (uint8_t*)&dummy + sizeof(uintptr_t));
}

void vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    uint32_t data[2] = { vertexCount, firstVertex };
    commandBuffer->stream.push_back((uint8_t)Opcode::Draw);
    commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)data, (uint8_t*)data + sizeof(data));
}

void vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    uint32_t data[2] = { indexCount, firstIndex };
    commandBuffer->stream.push_back((uint8_t)Opcode::DrawIndexed);
    commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)data, (uint8_t*)data + sizeof(data));
}

void vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    commandBuffer->stream.push_back((uint8_t)Opcode::DrawIndirect);
    commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&buffer, (uint8_t*)&buffer + sizeof(void*));
    commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&offset, (uint8_t*)&offset + sizeof(VkDeviceSize));
}

void vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    commandBuffer->stream.push_back((uint8_t)Opcode::DrawIndexedIndirect);
    commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&buffer, (uint8_t*)&buffer + sizeof(void*));
    commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&offset, (uint8_t*)&offset + sizeof(VkDeviceSize));
}

void vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) {
    if (pViewports && viewportCount > 0) {
        float data[4] = { pViewports[0].x, pViewports[0].y, pViewports[0].width, pViewports[0].height };
        commandBuffer->stream.push_back((uint8_t)Opcode::SetViewport);
        commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)data, (uint8_t*)data + sizeof(data));
    }
}

void vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) {
    if (pScissors && scissorCount > 0) {
        int32_t data[4] = { pScissors[0].offset.x, pScissors[0].offset.y, (int32_t)pScissors[0].extent.width, (int32_t)pScissors[0].extent.height };
        commandBuffer->stream.push_back((uint8_t)Opcode::SetScissor);
        commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)data, (uint8_t*)data + sizeof(data));
    }
}

void vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) {
    if (pRenderPassBegin) {
        commandBuffer->stream.push_back((uint8_t)Opcode::BeginRenderPass);
        commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&pRenderPassBegin->renderPass, (uint8_t*)&pRenderPassBegin->renderPass + sizeof(void*));
        commandBuffer->stream.insert(commandBuffer->stream.end(), (uint8_t*)&pRenderPassBegin->framebuffer, (uint8_t*)&pRenderPassBegin->framebuffer + sizeof(void*));
    }
}

void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
    if (commandBuffer) commandBuffer->stream.push_back((uint8_t)Opcode::EndRenderPass);
}

VkResult vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    return VK_SUCCESS;
}

VkResult vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    if (!pSubmits || submitCount == 0) return VK_SUCCESS;
    VkCommandBuffer cmdBuffer = pSubmits[0].pCommandBuffers[0];
    if (!cmdBuffer) return VK_SUCCESS;
    const uint8_t* reader = cmdBuffer->stream.data();
    size_t size = cmdBuffer->stream.size();
    size_t offset = 0;
    while (offset < size) {
        Opcode op = (Opcode)reader[offset];
        offset += sizeof(Opcode);

        switch (op) {
            case Opcode::BindPipeline: {
                VkPipeline pipeline = *(const VkPipeline*)(reader + offset);
                if (pipeline) glUseProgram(pipeline->gles_program);
                offset += sizeof(void*);
                break;
            }
            case Opcode::BindVertexBuffers: {
                VkBuffer buffer = *(const VkBuffer*)(reader + offset);
                if (buffer) glBindBuffer(GL_ARRAY_BUFFER, buffer->gles_buffer);
                offset += sizeof(void*);
                break;
            }
            case Opcode::BindIndexBuffer: {
                VkBuffer buffer = *(const VkBuffer*)(reader + offset);
                if (buffer) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->gles_buffer);
                offset += sizeof(void*);
                break;
            }
            case Opcode::BindDescriptorSets: {
                offset += sizeof(uintptr_t);
                break;
            }
            case Opcode::Draw: {
                const uint32_t* data = (const uint32_t*)(reader + offset);
                glDrawArrays(GL_TRIANGLES, data[1], data[0]);
                offset += sizeof(uint32_t) * 2;
                break;
            }
            case Opcode::DrawIndexed: {
                const uint32_t* data = (const uint32_t*)(reader + offset);
                glDrawElements(GL_TRIANGLES, data[0], GL_UNSIGNED_SHORT, (const void*)(uintptr_t)data[1]);
                offset += sizeof(uint32_t) * 2;
                break;
            }
            case Opcode::DrawIndirect: {
                VkBuffer buffer = *(const VkBuffer*)(reader + offset);
                offset += sizeof(void*);
                VkDeviceSize off = *(const VkDeviceSize*)(reader + offset);
                offset += sizeof(VkDeviceSize);
                if (buffer) glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->gles_buffer);
                glDrawArraysIndirect(GL_TRIANGLES, (const void*)(uintptr_t)off);
                break;
            }
            case Opcode::DrawIndexedIndirect: {
                VkBuffer buffer = *(const VkBuffer*)(reader + offset);
                offset += sizeof(void*);
                VkDeviceSize off = *(const VkDeviceSize*)(reader + offset);
                offset += sizeof(VkDeviceSize);
                if (buffer) glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer->gles_buffer);
                glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, (const void*)(uintptr_t)off);
                break;
            }
            case Opcode::SetViewport: {
                const float* data = (const float*)(reader + offset);
                glViewport((int)data[0], (int)data[1], (int)data[2], (int)data[3]);
                offset += sizeof(float) * 4;
                break;
            }
            case Opcode::SetScissor: {
                const int32_t* data = (const int32_t*)(reader + offset);
                glScissor(data[0], data[1], data[2], data[3]);
                offset += sizeof(int32_t) * 4;
                break;
            }
            case Opcode::BeginRenderPass: {
                VkRenderPass rp = *(const VkRenderPass*)(reader + offset);
                offset += sizeof(void*);
                VkFramebuffer fb = *(const VkFramebuffer*)(reader + offset);
                offset += sizeof(void*);
                glBindFramebuffer(GL_FRAMEBUFFER, fb ? fb->gles_fbo : 0);
                if (rp && rp->clear_color) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            }
            case Opcode::EndRenderPass: {
                break;
            }
        }
    }
    return VK_SUCCESS;
}
