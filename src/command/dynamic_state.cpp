#include "../struct/stcmd.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <cmath>

extern "C" {

void vkCmdSetViewport(VkCommandBuffer commandBuffer, int32_t firstViewport, int32_t viewportCount, const VkViewport* pViewports) {
    if (!commandBuffer || !pViewports || viewportCount <= 0) return;    
    VkViewport vp = pViewports[0];    
    commandBuffer->commands.push_back([vp, commandBuffer]() {
        int32_t renderTargetHeight = commandBuffer->renderTargetHeight;     
        glViewport(static_cast<GLint>(std::floor(vp.x)),
                   static_cast<GLint>(std::floor(renderTargetHeight - (vp.y + vp.height))),
                   static_cast<GLsizei>(std::floor(vp.width)),
                   static_cast<GLsizei>(std::floor(vp.height)));
        glDepthRangef(vp.minDepth, vp.maxDepth);
    });
}

void vkCmdSetScissor(VkCommandBuffer commandBuffer, int32_t firstScissor, int32_t scissorCount, const VkRect2D* pScissors) {
    if (!commandBuffer || !pScissors || scissorCount <= 0) return;    
    VkRect2D sc = pScissors[0];    
    commandBuffer->commands.push_back([sc]() {
        int32_t renderTargetHeight = commandBuffer->renderTargetHeight;     
        glScissor(static_cast<GLint>(sc.offset.x),
                  static_cast<GLint>(renderTargetHeight - (sc.offset.y + sc.extent.height)),
                  static_cast<GLsizei>(sc.extent.width),
                  static_cast<GLsizei>(sc.extent.height));
    });
}

void vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth) {
    if (!commandBuffer) return;
    glLineWidth(lineWidth);
}

void vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {
    if (!commandBuffer) return;
    glPolygonOffset(depthBiasConstantFactor, depthBiasSlopeFactor);
}

void vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4]) {
    if (!commandBuffer) return;
    glBlendColor(blendConstants[0], blendConstants[1], blendConstants[2], blendConstants[3]);
}

int32_t vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds) {
    if (!commandBuffer) return;
    if (glDepthBoundsEXT) {
        glDepthBoundsEXT(minDepthBounds, maxDepthBounds);
    } else {
        return 0;
    }
}

void vkCmdSetStencilReference(VkCommandBuffer commandBuffer, int32_t faceMask, int32_t reference) {
    if (!commandBuffer) return;
    if (faceMask & 1) {
        commandBuffer->stencilRefFront = reference;
        glStencilFuncSeparate(GL_FRONT, commandBuffer->stencilFuncFront, reference, commandBuffer->stencilMaskFront);
    }
    if (faceMask & 2) {
        commandBuffer->stencilRefBack = reference;
        glStencilFuncSeparate(GL_BACK, commandBuffer->stencilFuncBack, reference, commandBuffer->stencilMaskBack);
    }
}

void vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, int32_t faceMask, int32_t compareMask) {
    if (!commandBuffer) return;
    if (faceMask & 1) {
        commandBuffer->stencilMaskFront = compareMask;
        glStencilFuncSeparate(GL_FRONT, commandBuffer->stencilFuncFront, commandBuffer->stencilRefFront, compareMask);
    }
    if (faceMask & 2) {
        commandBuffer->stencilMaskBack = compareMask;
        glStencilFuncSeparate(GL_BACK, commandBuffer->stencilFuncBack, commandBuffer->stencilRefBack, compareMask);
    }
}

void vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, int32_t faceMask, int32_t writeMask) {
    if (!commandBuffer) return;
    if (faceMask & 1) glStencilMaskSeparate(GL_FRONT, writeMask);
    if (faceMask & 2) glStencilMaskSeparate(GL_BACK, writeMask);
}

void vkCmdSetCullMode(VkCommandBuffer commandBuffer, int32_t cullMode) {
    if (!commandBuffer) return;
    if (cullMode == 0) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
        if (cullMode == 1) glCullFace(GL_FRONT);
        else if (cullMode == 2) glCullFace(GL_BACK);
        else if (cullMode == 3) glCullFace(GL_FRONT_AND_BACK);
    }
}

void vkCmdSetFrontFace(VkCommandBuffer commandBuffer, int32_t frontFace) {
    if (!commandBuffer) return;
    if (frontFace == 0) glFrontFace(GL_CCW);
    else glFrontFace(GL_CW);
}

void vkCmdSetPrimitiveTopology(VkCommandBuffer commandBuffer, int32_t primitiveTopology) {
    if (!commandBuffer) return;
    commandBuffer->topology = primitiveTopology;
}

void vkCmdSetViewportWithCount(VkCommandBuffer commandBuffer, int32_t viewportCount, const void* pViewports) {
    if (!commandBuffer || !pViewports) return;
    const float* vp = (const float*)pViewports;
    for (int32_t i = 0; i < viewportCount; ++i) {
        glViewport((int32_t)vp[0], (int32_t)vp[1], (int32_t)vp[2], (int32_t)vp[3]);
        vp += 6;
    }
}

void vkCmdSetScissorWithCount(VkCommandBuffer commandBuffer, int32_t scissorCount, const void* pScissors) {
    if (!commandBuffer || !pScissors) return;
    const int32_t* sc = (const int32_t*)pScissors;
    for (int32_t i = 0; i < scissorCount; ++i) {
        glScissor(sc[0], sc[1], sc[2], sc[3]);
        sc += 4;
    }
}

void vkCmdSetDepthTestEnable(VkCommandBuffer commandBuffer, int32_t depthTestEnable) {
    if (!commandBuffer) return;
    if (depthTestEnable) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
}

void vkCmdSetDepthWriteEnable(VkCommandBuffer commandBuffer, int32_t depthWriteEnable) {
    if (!commandBuffer) return;
    glDepthMask(depthWriteEnable ? GL_TRUE : GL_FALSE);
}

void vkCmdSetDepthCompareOp(VkCommandBuffer commandBuffer, int32_t depthCompareOp) {
    if (!commandBuffer) return;
    int32_t func = GL_ALWAYS;
    switch (depthCompareOp) {
        case 0: func = GL_NEVER; break;
        case 1: func = GL_LESS; break;
        case 2: func = GL_EQUAL; break;
        case 3: func = GL_LEQUAL; break;
        case 4: func = GL_GREATER; break;
        case 5: func = GL_NOTEQUAL; break;
        case 6: func = GL_GEQUAL; break;
        case 7: func = GL_ALWAYS; break;
    }
    glDepthFunc(func);
}

int32_t vkCmdSetDepthBoundsTestEnable(VkCommandBuffer commandBuffer, int32_t depthBoundsTestEnable) {
    if (!commandBuffer) return;
    if (glDepthBoundsEXT) {
        if (depthBoundsTestEnable) {
            glEnable(GL_DEPTH_BOUNDS_TEST_EXT);
        } else {
            glDisable(GL_DEPTH_BOUNDS_TEST_EXT);
        }
    } else {
        return 0;
    }
}

void vkCmdSetStencilTestEnable(VkCommandBuffer commandBuffer, int32_t stencilTestEnable) {
    if (!commandBuffer) return;
    if (stencilTestEnable) glEnable(GL_STENCIL_TEST);
    else glDisable(GL_STENCIL_TEST);
}

void vkCmdSetStencilOp(VkCommandBuffer commandBuffer, int32_t faceMask, int32_t failOp, int32_t passOp, int32_t depthFailOp, int32_t compareOp) {
    if (!commandBuffer) return;
    int32_t sfail = GL_KEEP, spass = GL_KEEP, sdfail = GL_KEEP;
    switch (failOp) {
        case 0: sfail = GL_KEEP; break;
        case 1: sfail = GL_ZERO; break;
        case 2: sfail = GL_REPLACE; break;
        case 3: sfail = GL_INCR; break;
        case 4: sfail = GL_DECR; break;
        case 5: sfail = GL_INVERT; break;
        case 6: sfail = GL_INCR_WRAP; break;
        case 7: sfail = GL_DECR_WRAP; break;
    }
    switch (passOp) {
        case 0: spass = GL_KEEP; break;
        case 1: spass = GL_ZERO; break;
        case 2: spass = GL_REPLACE; break;
        case 3: spass = GL_INCR; break;
        case 4: spass = GL_DECR; break;
        case 5: spass = GL_INVERT; break;
        case 6: spass = GL_INCR_WRAP; break;
        case 7: spass = GL_DECR_WRAP; break;
    }
    switch (depthFailOp) {
        case 0: sdfail = GL_KEEP; break;
        case 1: sdfail = GL_ZERO; break;
        case 2: sdfail = GL_REPLACE; break;
        case 3: sdfail = GL_INCR; break;
        case 4: sdfail = GL_DECR; break;
        case 5: sdfail = GL_INVERT; break;
        case 6: sdfail = GL_INCR_WRAP; break;
        case 7: sdfail = GL_DECR_WRAP; break;
    }
    if (faceMask & 1) glStencilOpSeparate(GL_FRONT, sfail, sdfail, spass);
    if (faceMask & 2) glStencilOpSeparate(GL_BACK, sfail, sdfail, spass);
}

void vkCmdSetRasterizerDiscardEnable(VkCommandBuffer commandBuffer, int32_t rasterizerDiscardEnable) {
    if (!commandBuffer) return;
    if (rasterizerDiscardEnable) glEnable(GL_RASTERIZER_DISCARD);
    else glDisable(GL_RASTERIZER_DISCARD);
}

void vkCmdSetPrimitiveRestartEnable(VkCommandBuffer commandBuffer, int32_t primitiveRestartEnable) {
    if (!commandBuffer) return;
    if (primitiveRestartEnable) glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    else glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
}


}
