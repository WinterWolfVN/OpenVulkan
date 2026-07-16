#include <string.h>

struct FPAEntry {
    const char* name;
    PFN_vkVoidFunction func;
};

static const FPAEntry dispatch_table[] = {
    // [context.cpp] 
    {"vkCreateInstance", (PFN_vkVoidFunction)vkCreateInstance},
    {"vkCreateDevice", (PFN_vkVoidFunction)vkCreateDevice},
    {"vkEnumeratePhysicalDevices", (PFN_vkVoidFunction)vkEnumeratePhysicalDevices},
    {"vkGetPhysicalDeviceProperties", (PFN_vkVoidFunction)vkGetPhysicalDeviceProperties},
    {"vkGetPhysicalDeviceMemoryProperties", (PFN_vkVoidFunction)vkGetPhysicalDeviceMemoryProperties},
    {"vkCreateFramebuffer", (PFN_vkVoidFunction)vkCreateFramebuffer},
    {"vkDestroyFramebuffer", (PFN_vkVoidFunction)vkDestroyFramebuffer},

    // [buffer.cpp]
    {"vkCreateBuffer", (PFN_vkVoidFunction)vkCreateBuffer},
    {"vkDestroyBuffer", (PFN_vkVoidFunction)vkDestroyBuffer},
    {"vkAllocateMemory", (PFN_vkVoidFunction)vkAllocateMemory},
    {"vkFreeMemory", (PFN_vkVoidFunction)vkFreeMemory},
    {"vkBindBufferMemory", (PFN_vkVoidFunction)vkBindBufferMemory},
    {"vkMapMemory", (PFN_vkVoidFunction)vkMapMemory},
    {"vkUnmapMemory", (PFN_vkVoidFunction)vkUnmapMemory},
    {"vkCreateCommandPool", (PFN_vkVoidFunction)vkCreateCommandPool},
    {"vkDestroyCommandPool", (PFN_vkVoidFunction)vkDestroyCommandPool},
    {"vkAllocateCommandBuffers", (PFN_vkVoidFunction)vkAllocateCommandBuffers},
    {"vkFreeCommandBuffers", (PFN_vkVoidFunction)vkFreeCommandBuffers},    
    {"vkResetCommandBuffer", (PFN_vkVoidFunction)vkResetCommandBuffer},
    {"vkResetCommandPool", (PFN_vkVoidFunction)vkResetCommandPool},
    // [texture.cpp] 
    {"vkCreateImage", (PFN_vkVoidFunction)vkCreateImage},
    {"vkDestroyImage", (PFN_vkVoidFunction)vkDestroyImage},
    {"vkCreateImageView", (PFN_vkVoidFunction)vkCreateImageView},
    {"vkDestroyImageView", (PFN_vkVoidFunction)vkDestroyImageView},
    {"vkCreateSampler", (PFN_vkVoidFunction)vkCreateSampler},
    {"vkDestroySampler", (PFN_vkVoidFunction)vkDestroySampler},

    // [cmd_texture.cpp] 
    {"vkCmdCopyBufferToImage", (PFN_vkVoidFunction)vkCmdCopyBufferToImage},
    {"vkCmdCopyImageToBuffer", (PFN_vkVoidFunction)vkCmdCopyImageToBuffer},
    {"vkCmdBlitImage", (PFN_vkVoidFunction)vkCmdBlitImage}, 
    // [command.cpp]
    {"vkBeginCommandBuffer", (PFN_vkVoidFunction)vkBeginCommandBuffer},
    {"vkEndCommandBuffer", (PFN_vkVoidFunction)vkEndCommandBuffer},
    {"vkCmdBeginRenderPass", (PFN_vkVoidFunction)vkCmdBeginRenderPass},
    {"vkCmdEndRenderPass", (PFN_vkVoidFunction)vkCmdEndRenderPass},
    {"vkTrimCommandPool", (PFN_vkVoidFunction)vkTrimCommandPool},
    {"vkCmdPipelineBarrier", (PFN_vkVoidFunction)vkCmdPipelineBarrier},
    {"vkCmdPipelineBarrier2", (PFN_vkVoidFunction)vkCmdPipelineBarrier2},
    {"vkCmdExecuteCommands", (PFN_vkVoidFunction)vkCmdExecuteCommands},
    {"vkCmdBeginRendering", (PFN_vkVoidFunction)vkCmdBeginRendering},
    {"vkCmdEndRendering", (PFN_vkVoidFuntion)vkCmdEndRendering},
    {"vkQueueSubmit", (PFN_vkVoidFunction)vkQueueSubmit},
    {"vkQueueSubmit2", (PFN_vkVoidFuntion)vkQueueSubmit2},
    // [draw.cpp]
    {"vkCmdDraw", (PFN_vkVoidFunction)vkCmdDraw},
    {"vkCmdDrawIndexed", (PFN_vkVoidFunction)vkCmdDrawIndexed},
    {"vkCmdDrawIndirct", (PFN_vkVoidFunction)vkCmdDrawIndirect},
    {"vkCmdDrawIndexedIndirect", (PFN_vkVoidFunction)vkCmdDrawIndexedIndirect},
    {"vkCmdBindDescriptorSets", (PFN_vkVoidFunction)vkCmdBindDescriptorSets},
    {"vkCmdDispatch", (PFN_vkVoidFunction)vkCmdDispatch}, 
    {"vkCmdBindVertexBuffers", (PFN_vkVoidFunction)vkCmdBindVertexBuffers},
    {"vkCmdBindIndexBuffer", (PFN_vkVoidFunction)vkCmdBindIndexBuffer}, 
    {"vkCmdBindPipeline", (PFN_vkVoidFunction)vkCmdBindPipeline},
    // [dynamic_state.cpp]
    {"vkCmdSetViewport", (PFN_vkVoidFunction)vkCmdSetViewport},
    {"vkCmdSetScissor", (PFN_vkVoidFunction)vkCmdSetScissor},
    {"vkCmdSetLineWidth", (PFN_vkVoidFunction)vkCmdSetLineWidth},
    {"vkCmdSetDepthBias", (PFN_vkVoidFunction)vkCmdSetDepthBias},
    {"vkCmdSetBlendConstants", (PFN_vkVoidFunction)vkCmdSetBlendConstants},
    {"vkCmdSetDepthBounds", (PFN_vkVoidFunction)vkCmdSetDepthBounds},
    {"vkCmdSetStencilReference", (PFN_vkVoidFunction)vkCmdSetStencilReference},
    {"vkCmdSetStencilCompareMask", (PFN_vkVoidFunction)vkCmdSetStencilCompareMask},
    {"vkCmdSetStencilWriteMask", (PFN_vkVoidFunction)vkCmdSetStencilWriteMask},
    {"vkCmdSetCullMode", (PFN_vkVoidFunction)vkCmdSetCullMode},
    {"vkCmdSetFrontFace", (PFN_vkVoidFunction)vkCmdSetFrontFace},
    {"vkCmdSetPrimitiveTopology", (PFN_vkVoidFunction)vkCmdSetPrimitiveTopology},
    {"vkCmdSetViewportWithCount", (PFN_vkVoidFunction)vkCmdSetViewportWithCount},
    {"vkCmdSetScissorWithCount", (PFN_vkVoidFunction)vkCmdSetScissorWithCount},
    {"vkCmdSetDepthTestEnable", (PFN_vkVoidFunction)vkCmdSetDepthTestEnable},
    {"vkCmdSetDepthWriteEnable", (PFN_vkVoidFunction)vkCmdSetDepthWriteEnable},
    {"vkCmdSetDepthCompareOp", (PFN_vkVoidFunction)vkCmdSetDepthCompareOp},
    {"vkCmdSetDepthBoundsTestEnable", (PFN_vkVoidFunction)vkCmdSetDepthBoundsTestEnable},
    {"vkCmdSetStencilTestEnable", (PFN_vkVoidFunction)vkCmdSetStencilTestEnable},
    {"vkCmdSetStencilOp", (PFN_vkVoidFunction)vkCmdSetStencilOp},
    {"vkCmdSetRasterizerDiscardEnable", (PFN_vkVoidFunction)vkCmdSetRasterizerDiscardEnable},
    {"vkCmdSetPrimitiveRestartEnable", (PFN_vkVoidFunction)vkCmdSetPrimitiveRestartEnable},

    // [descriptor.cpp] 
    {"vkCreateDescriptorSetLayout", (PFN_vkVoidFunction)vkCreateDescriptorSetLayout},
    {"vkDestroyDescriptorSetLayout", (PFN_vkVoidFunction)vkDestroyDescriptorSetLayout},
    {"vkCreateDescriptorPool", (PFN_vkVoidFunction)vkCreateDescriptorPool},
    {"vkDestroyDescriptorPool", (PFN_vkVoidFunction)vkDestroyDescriptorPool},
    {"vkAllocateDescriptorSets", (PFN_vkVoidFunction)vkAllocateDescriptorSets},
    {"vkFreeDescriptorSets", (PFN_vkVoidFunction)vkFreeDescriptorSets},
    {"vkUpdateDescriptorSets", (PFN_vkVoidFunction)vkUpdateDescriptorSets},

    // [shader.cpp] 
    {"vkCreateShaderModule", (PFN_vkVoidFunction)vkCreateShaderModule},
    {"vkDestroyShaderModule", (PFN_vkVoidFunction)vkDestroyShaderModule},
    // [pipeline.cpp]
    {"vkCreateGraphicsPipelines", (PFN_vkVoidFunction)vkCreateGraphicsPipelines},
    {"vkCreateComputePipelines", (PFN_vkVoidFunction)vkCreateComputePipelines},
    {"vkDestroyPipeline", (PFN_vkVoidFunction)vkDestroyPipeline},

    // [sync.cpp]
    {"vkCreateFence", (PFN_vkVoidFunction)vkCreateFence},
    {"vkDestroyFence", (PFN_vkVoidFunction)vkDestroyFence},
    {"vkResetFences", (PFN_vkVoidFunction)vkResetFences},
    {"vkWaitForFences", (PFN_vkVoidFunction)vkWaitForFences},
    {"vkGetFenceStatus", (PFN_vkVoidFunction)vkGetFenceStatus},
    {"vkCreateEvent", (PFN_vkVoidFunction)vkCreateEvent},
    {"vkCreateEvent", (PFN_vkVoidFunction)vkCreateEvent},
    {"vkDestroyEvent", (PFN_vkVoidFunction)vkDestroyEvent},
    {"vkSetEvent", (PFN_vkVoidFunction)vkSetEvent},
    {"vkResetEvent", (PFN_vkVoidFunction)vkResetEvent},
    {"vkGetEventStatus", (PFN_vkVoidFunction)vkGetEventStatus},
    {"vkQueueWaitIdle", (PFN_vkVoidFunction)vkQueueWaitIdle},
    {"vkDeviceWaitIdle", (PFN_vkVoidFunction)vkDeviceWaitIdle},
    {"vkCmdSetEvent2", (PFN_vkVoidFunction)vkCmdSetEvent2},
    {"vkCmdResetEvent2", (PFN_vkCmdResetEvent2)vkCmdResetEvent2},
    {"vkCmdWaitEvents2", (PFN_vkCmdWaitEvents2)vkCmdWaitEvents2},
    // [sema.cpp]
    {"vkCreateSemaphore", (PFN_vkVoidFunction)vkCreateSemaphore},
    {"vkDestroySemaphore", (PFN_vkVoidFunction)vkDestroySemaphore},
    {"vkGetSemaphoreCounterValue", (PFN_vkVoidFunction)vkGetSemaphoreCounterValue},
    {"vkSignalSemaphore", (PFN_vkVoidFunction)vkSignalSemaphore},
    {"vkWaitSemaphores", (PFN_vkVoidFunction)vkWaitSemaphores}
};

extern "C"
PFN_vkVoidFunction VkGetDeviceProcAddr(const char* pName) {
    if (!pName)
        return nullptr;
    for (const auto& entry : dispatch_table) {
        if (strcmp(entry.name, pName) == 0)
            return entry.func;
    }
    return nullptr;
}
