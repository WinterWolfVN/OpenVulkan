#include "stcmd.h"
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

void vkTrimCommandPool(VkDevice device, VkCommandPool commandPool, uint32_t flags) {
    if (!device || !commandPool) return;
}

void vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, uint32_t srcStageMask, uint32_t dstStageMask, uint32_t dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    if (!commandBuffer) return;
    if (memoryBarrierCount > MAX_BARRIERS || bufferMemoryBarrierCount > MAX_BARRIERS || imageMemoryBarrierCount > MAX_BARRIERS) {
        return;
    }
    if (memoryBarrierCount > 0 && !pMemoryBarriers) return;
    if (bufferMemoryBarrierCount > 0 && !pBufferMemoryBarriers) return;
    if (imageMemoryBarrierCount > 0 && !pImageMemoryBarriers) return;
    std::vector<VkMemoryBarrier2> memoryBarriers(memoryBarrierCount);
    std::vector<VkBufferMemoryBarrier2> bufferBarriers(bufferMemoryBarrierCount);
    std::vector<VkImageMemoryBarrier2> imageBarriers(imageMemoryBarrierCount);
    for (uint32_t i = 0; i < memoryBarrierCount; ++i) {
        const VkMemoryBarrier& src = pMemoryBarriers[i];
        VkMemoryBarrier2& dst = memoryBarriers[i];
        dst.pNext = src.pNext;
        dst.sType = src.sType;
        dst.srcStageMask = srcStageMask;
        dst.srcAccessMask = src.srcAccessMask;
        dst.dstStageMask = dstStageMask;
        dst.dstAccessMask = src.dstAccessMask;
    }
    for (uint32_t i = 0; i < bufferMemoryBarrierCount; ++i) {
        const VkBufferMemoryBarrier& src = pBufferMemoryBarriers[i];
        VkBufferMemoryBarrier2& dst = bufferBarriers[i];
        dst.pNext = src.pNext;
        dst.sType = src.sType;
        dst.buffer = src.buffer;
        dst.offset = src.offset;
        dst.size = src.size;
        dst.srcStageMask = srcStageMask;
        dst.srcAccessMask = src.srcAccessMask;
        dst.dstStageMask = dstStageMask;
        dst.dstAccessMask = src.dstAccessMask;
        dst.srcQueueFamilyIndex = src.srcQueueFamilyIndex;
        dst.dstQueueFamilyIndex = src.dstQueueFamilyIndex;
    }
    for (uint32_t i = 0; i < imageMemoryBarrierCount; ++i) {
        const VkImageMemoryBarrier& src = pImageMemoryBarriers[i];
        VkImageMemoryBarrier2& dst = imageBarriers[i];
        dst.pNext = src.pNext;
        dst.sType = src.sType;
        dst.image = src.image;
        dst.srcStageMask = srcStageMask;
        dst.srcAccessMask = src.srcAccessMask;
        dst.dstStageMask = dstStageMask;
        dst.dstAccessMask = src.dstAccessMask;
        dst.oldLayout = src.oldLayout;
        dst.newLayout = src.newLayout;
        dst.srcQueueFamilyIndex = src.srcQueueFamilyIndex;
        dst.dstQueueFamilyIndex = src.dstQueueFamilyIndex;
        dst.subresourceRange = src.subresourceRange;
    }
    commandBuffer->commands.push_back([memoryBarriers = std::move(memoryBarriers), bufferBarriers = std::move(bufferBarriers), imageBarriers = std::move(imageBarriers), dependencyFlags]() {
            for (const auto& barrier : memoryBarriers) {
                (void)barrier;
            }
            for (const auto& barrier : bufferBarriers) {
                (void)barrier;
            }
            for (const auto& barrier : imageBarriers) {
                (void)barrier;
            }
            (void)dependencyFlags;
        }
    );
}

void vkCmdPipelineBarrier2(VkCommandBuffer commandBuffer, const VkDependencyInfo* pDependencyInfo) {
    if (!commandBuffer || !pDependencyInfo) return;
    if (pDependencyInfo->memoryBarrierCount > MAX_BARRIERS || pDependencyInfo->bufferMemoryBarrierCount > MAX_BARRIERS || pDependencyInfo->imageMemoryBarrierCount > MAX_BARRIERS) {
        return;
    }
    if (pDependencyInfo->memoryBarrierCount > 0 && !pDependencyInfo->pMemoryBarriers) {
        return;
    }
    if (pDependencyInfo->bufferMemoryBarrierCount > 0 && !pDependencyInfo->pBufferMemoryBarriers) {
        return;
    }
    if (pDependencyInfo->imageMemoryBarrierCount > 0 && !pDependencyInfo->pImageMemoryBarriers) {
        return;
    }
    const uint32_t memoryBarrierCount = pDependencyInfo->memoryBarrierCount;
    const uint32_t bufferMemoryBarrierCount = pDependencyInfo->bufferMemoryBarrierCount;
    const uint32_t imageMemoryBarrierCount = pDependencyInfo->imageMemoryBarrierCount;
    std::vector<VkMemoryBarrier2> memoryBarriers(memoryBarrierCount);
    std::vector<VkBufferMemoryBarrier2> bufferBarriers(bufferMemoryBarrierCount);
    std::vector<VkImageMemoryBarrier2> imageBarriers(imageMemoryBarrierCount);
    for (uint32_t i = 0; i < memoryBarrierCount; ++i) {
        memoryBarriers[i] = pDependencyInfo->pMemoryBarriers[i];
    }
    for (uint32_t i = 0; i < bufferMemoryBarrierCount; ++i) {
        bufferBarriers[i] = pDependencyInfo->pBufferMemoryBarriers[i];
    }
    for (uint32_t i = 0; i < imageMemoryBarrierCount; ++i) {
        imageBarriers[i] = pDependencyInfo->pImageMemoryBarriers[i];
    }
    const uint32_t dependencyFlags = pDependencyInfo->dependencyFlags;
    commandBuffer->commands.push_back([memoryBarriers = std::move(memoryBarriers), bufferBarriers = std::move(bufferBarriers), imageBarriers = std::move(imageBarriers),
         dependencyFlags]() {
            for (const auto& barrier : memoryBarriers) {
                (void)barrier;
            }
            for (const auto& barrier : bufferBarriers) {
                (void)barrier;
            }
            for (const auto& barrier : imageBarriers) {
                (void)barrier;
            }
            (void)dependencyFlags;
        }
    );
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
