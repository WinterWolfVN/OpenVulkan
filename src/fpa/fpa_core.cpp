#include <string.h>

struct FPAEntry {
    const char* name;
    PFN_vkVoidFunction func;
};

static const FPAEntry dispatch_table[] = {

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

    //[texture.cpp] 
    {"vkCreateImage", (PFN_vkVoidFunction)vkCreateImage},
    {"vkDestroyImage", (PFN_vkVoidFunction)vkDestroyImage},
    {"vkCreateImageView", (PFN_vkVoidFunction)vkCreateImageView},
    {"vkDestroyImageView", (PFN_vkVoidFunction)vkDestroyImageView},
    {"vkCreateSampler", (PFN_vkVoidFunction)vkCreateSampler},
    {"vkDestroySampler", (PFN_vkVoidFunction)vkDestroySampler},

    // [draw.cpp]
    {"vkBeginCommandBuffer", (PFN_vkVoidFunction)vkBeginCommandBuffer},
    {"vkEndCommandBuffer", (PFN_vkVoidFunction)vkEndCommandBuffer},
    {"vkCmdBeginRenderPass", (PFN_vkVoidFunction)vkCmdBeginRenderPass},
    {"vkCmdEndRenderPass", (PFN_vkVoidFunction)vkCmdEndRenderPass},    
    {"vkCmdBindVertexBuffers", (PFN_vkVoidFunction)vkCmdBindVertexBuffers},
    {"vkCmdBindIndexBuffer", (PFN_vkVoidFunction)vkCmdBindIndexBuffer},
    {"vkCmdSetViewport", (PFN_vkVoidFunction)vkCmdSetViewport},
    {"vkCmdSetScissor", (PFN_vkVoidFunction)vkCmdSetScissor},
    {"vkCmdDraw", (PFN_vkVoidFunction)vkCmdDraw},
    {"vkCmdDrawIndexed", (PFN_vkVoidFunction)vkCmdDrawIndexed},
    {"vkCmdDrawIndirect", (PFN_vkVoidFunction)vkCmdDrawIndirect},
    {"vkCmdDrawIndexedIndirect", (PFN_vkVoidFunction)vkCmdDrawIndexedIndirect},
    {"vkCmdBindPipeline", (PFN_vkVoidFunction)vkCmdBindPipeline},
    {"vkCmdDispatch", (PFN_vkVoidFunction)vkCmdDispatch}, 
    {"vkCmdBindDescriptorSets", (PFN_vkVoidFunction)vkCmdBindDescriptorSets},
    {"vkQueueSubmit", (PFN_vkVoidFunction)vkQueueSubmit},    
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

