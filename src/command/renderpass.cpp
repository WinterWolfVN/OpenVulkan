#include "../struct/stdraw.h"
#include <new>
#include <cstring>

extern "C" {

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

int32_t vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const void* pAllocator, VkRenderPass* pRenderPass) {
    if (!device || !pCreateInfo || !pRenderPass) return -3;    
    auto* rp = new(std::nothrow) VkRenderPass_T();
    if (!rp) return -3;    
    std::memset(rp, 0, sizeof(*rp));
    rp->renderPassId = (int64_t)rp;
    rp->attachmentCount = pCreateInfo->attachmentCount;
    rp->subpassCount = pCreateInfo->subpassCount;
    rp->dependencyCount = pCreateInfo->dependencyCount;    
    if (rp->attachmentCount > 0 && pCreateInfo->pAttachments) {
        int32_t count = rp->attachmentCount > MAX_ATTACHMENTS ? MAX_ATTACHMENTS : rp->attachmentCount;
        for (int32_t i = 0; i < count; ++i) {
            rp->attachments[i] = pCreateInfo->pAttachments[i];
        }
    }    
    if (rp->subpassCount > 0 && pCreateInfo->pSubpasses) {
        int32_t count = rp->subpassCount > MAX_SUBPASSES ? MAX_SUBPASSES : rp->subpassCount;
        for (int32_t i = 0; i < count; ++i) {
            rp->subpasses[i] = pCreateInfo->pSubpasses[i];            
            if (pCreateInfo->pSubpasses[i].pInputAttachments && pCreateInfo->pSubpasses[i].inputAttachmentCount > 0) {
                auto* input = new(std::nothrow) VkAttachmentReference[pCreateInfo->pSubpasses[i].inputAttachmentCount];
                if (input) {
                    for (int32_t j = 0; j < pCreateInfo->pSubpasses[i].inputAttachmentCount; ++j) {
                        input[j] = pCreateInfo->pSubpasses[i].pInputAttachments[j];
                    }
                    rp->subpasses[i].pInputAttachments = input;
                }
            }            
            if (pCreateInfo->pSubpasses[i].pColorAttachments && pCreateInfo->pSubpasses[i].colorAttachmentCount > 0) {
                auto* color = new(std::nothrow) VkAttachmentReference[pCreateInfo->pSubpasses[i].colorAttachmentCount];
                if (color) {
                    for (int32_t j = 0; j < pCreateInfo->pSubpasses[i].colorAttachmentCount; ++j) {
                        color[j] = pCreateInfo->pSubpasses[i].pColorAttachments[j];
                    }
                    rp->subpasses[i].pColorAttachments = color;
                }
            }            
            if (pCreateInfo->pSubpasses[i].pResolveAttachments && pCreateInfo->pSubpasses[i].colorAttachmentCount > 0) {
                auto* resolve = new(std::nothrow) VkAttachmentReference[pCreateInfo->pSubpasses[i].colorAttachmentCount];
                if (resolve) {
                    for (int32_t j = 0; j < pCreateInfo->pSubpasses[i].colorAttachmentCount; ++j) {
                        resolve[j] = pCreateInfo->pSubpasses[i].pResolveAttachments[j];
                    }
                    rp->subpasses[i].pResolveAttachments = resolve;
                }
            }            
            if (pCreateInfo->pSubpasses[i].pDepthStencilAttachment) {
                auto* depth = new(std::nothrow) VkAttachmentReference;
                if (depth) {
                    *depth = *pCreateInfo->pSubpasses[i].pDepthStencilAttachment;
                    rp->subpasses[i].pDepthStencilAttachment = depth;
                }
            }            
            if (pCreateInfo->pSubpasses[i].pPreserveAttachments && pCreateInfo->pSubpasses[i].preserveAttachmentCount > 0) {
                auto* preserve = new(std::nothrow) int32_t[pCreateInfo->pSubpasses[i].preserveAttachmentCount];
                if (preserve) {
                    for (int32_t j = 0; j < pCreateInfo->pSubpasses[i].preserveAttachmentCount; ++j) {
                        preserve[j] = pCreateInfo->pSubpasses[i].pPreserveAttachments[j];
                    }
                    rp->subpasses[i].pPreserveAttachments = preserve;
                }
            }
        }
    }    
    if (rp->dependencyCount > 0 && pCreateInfo->pDependencies) {
        int32_t count = rp->dependencyCount > MAX_SUBPASS_DEPENDENCIES ? MAX_SUBPASS_DEPENDENCIES : rp->dependencyCount;
        for (int32_t i = 0; i < count; ++i) {
            rp->dependencies[i] = pCreateInfo->pDependencies[i];
        }
    }    
    *pRenderPass = rp;
    return 0;
}

void vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const void* pAllocator) {
    if (!renderPass) return;    
    for (int32_t i = 0; i < renderPass->subpassCount; ++i) {
        delete[] renderPass->subpasses[i].pInputAttachments;
        delete[] renderPass->subpasses[i].pColorAttachments;
        delete[] renderPass->subpasses[i].pResolveAttachments;
        delete renderPass->subpasses[i].pDepthStencilAttachment;
        delete[] renderPass->subpasses[i].pPreserveAttachments;
    }    
    delete renderPass;
}

void vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, int32_t* pGranularity) {
    if (!device || !pGranularity) return;
    pGranularity[0] = 1;
    pGranularity[1] = 1;
}

}
