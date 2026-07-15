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

}
