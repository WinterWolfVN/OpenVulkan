#include "../struct/stcmd.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <vector>
#include <utility>

extern "C" {

int32_t vkBeginCommandBuffer(
    VkCommandBuffer commandBuffer,
    const VkCommandBufferBeginInfo* pBeginInfo)
{
    if (!commandBuffer)
        return -3;
    commandBuffer->commands.clear();
    commandBuffer->recording = true;
    if (pBeginInfo)
        commandBuffer->flags = pBeginInfo->flags;
    return 0;
}

int32 vkEndCommandBuffer(
    VkCommandBuffer commandBuffer)
{
    if (!commandBuffer)
        return -3;
    if (!commandBuffer->recording)
        return -3;
    commandBuffer->recording = false;
    return 0;
}

void vkTrimCommandPool(VkDevice device, VkCommandPool commandPool, int32_t flags) {
    if (!device || !commandPool) return;
}

void vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, int32_t srcStageMask, int32_t dstStageMask, int32_t dependencyFlags, int32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, int32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, int32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    if (!commandBuffer) return;    
    for (int32_t i = 0; i < memoryBarrierCount && commandBuffer->memoryBarrierCount < MAX_BARRIERS; ++i) {
        VkMemoryBarrier2 b;
        std::memset(&b, 0, sizeof(b));
        b.srcStageMask = srcStageMask;
        b.dstStageMask = dstStageMask;
        b.srcAccessMask = pMemoryBarriers[i].srcAccessMask;
        b.dstAccessMask = pMemoryBarriers[i].dstAccessMask;
        commandBuffer->memoryBarriers[commandBuffer->memoryBarrierCount++] = b;
    }    
    for (int32_t i = 0; i < bufferMemoryBarrierCount && commandBuffer->bufferBarrierCount < MAX_BARRIERS; ++i) {
        VkBufferMemoryBarrier2 b;
        std::memset(&b, 0, sizeof(b));
        b.buffer = pBufferMemoryBarriers[i].buffer;
        b.offset = pBufferMemoryBarriers[i].offset;
        b.size = pBufferMemoryBarriers[i].size;
        b.srcStageMask = srcStageMask;
        b.dstStageMask = dstStageMask;
        b.srcAccessMask = pBufferMemoryBarriers[i].srcAccessMask;
        b.dstAccessMask = pBufferMemoryBarriers[i].dstAccessMask;
        b.srcQueueFamilyIndex = pBufferMemoryBarriers[i].srcQueueFamilyIndex;
        b.dstQueueFamilyIndex = pBufferMemoryBarriers[i].dstQueueFamilyIndex;
        commandBuffer->bufferBarriers[commandBuffer->bufferBarrierCount++] = b;
    }    
    for (int32_t i = 0; i < imageMemoryBarrierCount && commandBuffer->imageBarrierCount < MAX_BARRIERS; ++i) {
        VkImageMemoryBarrier2 b;
        std::memset(&b, 0, sizeof(b));
        b.image = pImageMemoryBarriers[i].image;
        b.srcStageMask = srcStageMask;
        b.dstStageMask = dstStageMask;
        b.srcAccessMask = pImageMemoryBarriers[i].srcAccessMask;
        b.dstAccessMask = pImageMemoryBarriers[i].dstAccessMask;
        b.oldLayout = pImageMemoryBarriers[i].oldLayout;
        b.newLayout = pImageMemoryBarriers[i].newLayout;
        b.srcQueueFamilyIndex = pImageMemoryBarriers[i].srcQueueFamilyIndex;
        b.dstQueueFamilyIndex = pImageMemoryBarriers[i].dstQueueFamilyIndex;
        b.subresourceRange = pImageMemoryBarriers[i].subresourceRange;
        commandBuffer->imageBarriers[commandBuffer->imageBarrierCount++] = b;
    }
}

void vkCmdPipelineBarrier2(VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo) {
    if (!commandBuffer || !pDependencyInfo) return;    
    for (int32_t i = 0; i < pDependencyInfo->memoryBarrierCount && commandBuffer->memoryBarrierCount < MAX_BARRIERS; ++i) {
        commandBuffer->memoryBarriers[commandBuffer->memoryBarrierCount++] = pDependencyInfo->pMemoryBarriers[i];
    }    
    for (int32_t i = 0; i < pDependencyInfo->bufferMemoryBarrierCount && commandBuffer->bufferBarrierCount < MAX_BARRIERS; ++i) {
        commandBuffer->bufferBarriers[commandBuffer->bufferBarrierCount++] = pDependencyInfo->pBufferMemoryBarriers[i];
    }    
    for (int32_t i = 0; i < pDependencyInfo->imageMemoryBarrierCount && commandBuffer->imageBarrierCount < MAX_BARRIERS; ++i) {
        commandBuffer->imageBarriers[commandBuffer->imageBarrierCount++] = pDependencyInfo->pImageMemoryBarriers[i];
    }
}

void vkCmdExecuteCommands(VkCommandBuffer commandBuffer, int32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    if (!commandBuffer || !pCommandBuffers) return;
}

void vkCmdBeginRendering(VkCommandBuffer commandBuffer, const VkRenderingInfo* pRenderingInfo) {
    if (!commandBuffer || !pRenderingInfo) return;    
    int32_t x = pRenderingInfo->renderAreaX;
    int32_t y = pRenderingInfo->renderAreaY;
    int32_t w = pRenderingInfo->renderAreaWidth;
    int32_t h = pRenderingInfo->renderAreaHeight;    
    float c0 = 0, c1 = 0, c2 = 0, c3 = 0;
    float clearDepth = 1.0f;
    int32_t clearStencil = 0;
    uint32_t clearMask = 0;    
    if (pRenderingInfo->pColorAttachments && pRenderingInfo->colorAttachmentCount > 0) {
        for (int32_t i = 0; i < pRenderingInfo->colorAttachmentCount; ++i) {
            if (pRenderingInfo->pColorAttachments[i].loadOp == 0) {
                c0 = pRenderingInfo->pColorAttachments[i].clearValue[0];
                c1 = pRenderingInfo->pColorAttachments[i].clearValue[1];
                c2 = pRenderingInfo->pColorAttachments[i].clearValue[2];
                c3 = pRenderingInfo->pColorAttachments[i].clearValue[3];
                clearMask |= GL_COLOR_BUFFER_BIT;
                break;
            }
        }
    }    
    if (pRenderingInfo->pDepthAttachment && pRenderingInfo->pDepthAttachment->loadOp == 0) {
        clearDepth = pRenderingInfo->pDepthAttachment->clearValue[0];
        clearMask |= GL_DEPTH_BUFFER_BIT;
    }    
    if (pRenderingInfo->pStencilAttachment && pRenderingInfo->pStencilAttachment->loadOp == 0) {
        clearStencil = (int32_t)pRenderingInfo->pStencilAttachment->clearValue[0];
        clearMask |= GL_STENCIL_BUFFER_BIT;
    }   
    commandBuffer->commands.push_back([x, y, w, h, c0, c1, c2, c3, clearDepth, clearStencil, clearMask]() {
        glViewport(x, y, w, h);
        glScissor(x, y, w, h);
        if (clearMask != 0) {
            if (clearMask & GL_COLOR_BUFFER_BIT) glClearColor(c0, c1, c2, c3);
            if (clearMask & GL_DEPTH_BUFFER_BIT) glClearDepthf(clearDepth);
            if (clearMask & GL_STENCIL_BUFFER_BIT) glClearStencil(clearStencil);
            glClear(clearMask);
        }
    });
}

void vkCmdEndRendering(VkCommandBuffer commandBuffer) {
    if (!commandBuffer) return;
}

int32_t vkQueueSubmit(VkQueue queue, int32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    if (!queue ||!pSubmits || submitCount <= 0) return -3;    
    for (int32_t i = 0; i < submitCount; ++i) {
        for (int32_t j = 0; j < pSubmits[i].commandBufferCount; ++j) {
            VkCommandBuffer cmd = pSubmits[i].pCommandBuffers[j];
            if (cmd) {
                for (const auto& command : cmd->commands) {
                    command();                     
                }
                cmd->commands.clear();
            }
        }
        if (fence) { fence->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0); fence->signaled = 0; }
    }   
    glFlush();
    return 0;
}
   } 

int32_t vkQueueSubmit2(VkQueue queue, int32_t submitCount, const VkSubmitInfo2* pSubmits, VkFence fence) {
    if (!queue) return -3;    
    if (submitCount > 0 && pSubmits) {
        for (int32_t i = 0; i < submitCount; ++i) {
            if (pSubmits[i].commandBufferInfoCount > 0 && pSubmits[i].pCommandBufferInfos) {
                for (int32_t j = 0; j < pSubmits[i].commandBufferInfoCount; ++j) {
                    VkCommandBuffer cb = pSubmits[i].pCommandBufferInfos[j].commandBuffer;
                    if (cb) {
                        cb->memoryBarrierCount = 0;
                        cb->bufferBarrierCount = 0;
                        cb->imageBarrierCount = 0;
                    }
                }
            }
        }
    }
    return 0;
}
