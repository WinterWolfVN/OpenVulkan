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
    {"vkCreatePipelineLayout", (PFN_vkVoidFunction)vkCreatePipelineLayout},
    {"vkDestroyPipelineLayout", (PFN_vkVoidFunction)vkDestroyPipelineLayout},
    {"vkCreateGraphicsPipelines", (PFN_vkVoidFunction)vkCreateGraphicsPipelines},
    {"vkDestroyPipeline", (PFN_vkVoidFunction)vkDestroyPipeline},

    // [command.cpp]
    {"vkCreateCommandPool", (PFN_vkVoidFunction)vkCreateCommandPool},
    {"vkDestroyCommandPool", (PFN_vkVoidFunction)vkDestroyCommandPool},
    {"vkAllocateCommandBuffers", (PFN_vkVoidFunction)vkAllocateCommandBuffers},
    {"vkFreeCommandBuffers", (PFN_vkVoidFunction)vkFreeCommandBuffers},
    {"vkBeginCommandBuffer", (PFN_vkVoidFunction)vkBeginCommandBuffer},
    {"vkEndCommandBuffer", (PFN_vkVoidFunction)vkEndCommandBuffer},
    {"vkResetCommandBuffer", (PFN_vkVoidFunction)vkResetCommandBuffer},
    {"vkResetCommandPool", (PFN_vkVoidFunction)vkResetCommandPool},
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
    {"vkCmdBindDescriptorSets", (PFN_vkVoidFunction)vkCmdBindDescriptorSets},
    {"vkQueueSubmit", (PFN_vkVoidFunction)vkQueueSubmit},

    // [shader.cpp]
    {"vkCreateShaderModule", (PFN_vkVoidFunction)vkCreateShaderModule},
    {"vkDestroyShaderModule", (PFN_vkVoidFunction)vkDestroyShaderModule},
    {"vkCreateDescriptorSetLayout", (PFN_vkVoidFunction)vkCreateDescriptorSetLayout},
    {"vkDestroyDescriptorSetLayout", (PFN_vkVoidFunction)vkDestroyDescriptorSetLayout},
    {"vkCreateDescriptorPool", (PFN_vkVoidFunction)vkCreateDescriptorPool},
    {"vkDestroyDescriptorPool", (PFN_vkVoidFunction)vkDestroyDescriptorPool},
    {"vkAllocateDescriptorSets", (PFN_vkVoidFunction)vkAllocateDescriptorSets},
    {"vkFreeDescriptorSets", (PFN_vkVoidFunction)vkFreeDescriptorSets},
    {"vkUpdateDescriptorSets", (PFN_vkVoidFunction)vkUpdateDescriptorSets},

    // [screen.cpp]
    {"vkCreateSwapchainKHR", (PFN_vkVoidFunction)vkCreateSwapchainKHR},
    {"vkDestroySwapchainKHR", (PFN_vkVoidFunction)vkDestroySwapchainKHR},
    {"vkGetSwapchainImagesKHR", (PFN_vkVoidFunction)vkGetSwapchainImagesKHR},
    {"vkAcquireNextImageKHR", (PFN_vkVoidFunction)vkAcquireNextImageKHR},
    {"vkQueuePresentKHR", (PFN_vkVoidFunction)vkQueuePresentKHR},

    // [context.cpp]
    {"vkCreateInstance", (PFN_vkVoidFunction)vkCreateInstance},
    {"vkCreateDevice", (PFN_vkVoidFunction)vkCreateDevice},
    {"vkEnumeratePhysicalDevices", (PFN_vkVoidFunction)vkEnumeratePhysicalDevices},
    {"vkGetPhysicalDeviceProperties", (PFN_vkVoidFunction)vkGetPhysicalDeviceProperties},
    {"vkGetPhysicalDeviceMemoryProperties", (PFN_vkVoidFunction)vkGetPhysicalDeviceMemoryProperties},
    {"vkCreateFramebuffer", (PFN_vkVoidFunction)vkCreateFramebuffer},
    {"vkDestroyFramebuffer", (PFN_vkVoidFunction)vkDestroyFramebuffer},
};

extern "C"
PFN_vkVoidFunction get_vulkan_proc_addr(const char* pName) {

    if (!pName)
        return nullptr;

    for (const auto& entry : dispatch_table) {

        if (strcmp(entry.name, pName) == 0)
            return entry.func;
    }

    return nullptr;
}
