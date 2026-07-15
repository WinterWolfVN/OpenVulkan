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

void vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, int32_t contents) {
    if (!commandBuffer || !pRenderPassBegin) return;    
    int32_t fbo = pRenderPassBegin->framebuffer ? pRenderPassBegin->framebuffer->fbo : 0;
    int32_t renderTargetHeight = pRenderPassBegin->framebuffer ? pRenderPassBegin->framebuffer->height : 0;
    VkRect2D area = pRenderPassBegin->renderArea;    
    std::vector<VkClearValue> clearValues;
    if (pRenderPassBegin->clearValueCount > 0 && pRenderPassBegin->pClearValues) {
        clearValues.assign(pRenderPassBegin->pClearValues, pRenderPassBegin->pClearValues + pRenderPassBegin->clearValueCount);
    }    
    commandBuffer->commands.push_back([fbo, area, renderTargetHeight, cvs = std::move(clearValues)]() {
        glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(fbo));                         
        glScissor(static_cast<GLint>(area.offset.x),
                  static_cast<GLint>(renderTargetHeight - (area.offset.y + area.extent.height)),
                  static_cast<GLsizei>(area.extent.width),
                  static_cast<GLsizei>(area.extent.height));                  
        if (!cvs.empty()) {
            GLbitfield clearMask = 0;            
            glClearColor(cvs[0].color.float32[0], cvs[0].color.float32[1], cvs[0].color.float32[2], cvs[0].color.float32[3]);
            clearMask |= GL_COLOR_BUFFER_BIT;            
            if (cvs.size() > 1) {
                glClearDepthf(cvs[1].depthStencil.depth);
                glClearStencil(cvs[1].depthStencil.stencil);
                clearMask |= (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            }            
            glClear(clearMask);
        }
    });
}

void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
    if (!commandBuffer) return;    
    commandBuffer->commands.push_back([]() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
}

int32_t vkQueueSubmit(VkQueue queue, int32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    if (!pSubmits || submitCount <= 0) return -3;    
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
    }   
    glFlush();
    return 0;
}
   } 
