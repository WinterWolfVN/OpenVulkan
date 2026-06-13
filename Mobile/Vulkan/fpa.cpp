#include <string.h>

struct FPAEntry { const char* name; PFN_vkVoidFunction func; };

static const FPAEntry dispatch_table[] = {
    // [buffer.cpp]
    {"vkCreateBuffer", (PFN_vkVoidFunction)vkCreateBuffer}, {"vkDestroyBuffer", (PFN_vkDestroyBuffer)vkDestroyBuffer},
    {"vkAllocateMemory", (PFN_vkAllocateMemory)vkAllocateMemory}, {"vkFreeMemory", (PFN_vkFreeMemory)vkFreeMemory},
    {"vkBindBufferMemory", (PFN_vkBindBufferMemory)vkBindBufferMemory}, {"vkMapMemory", (PFN_vkMapMemory)vkMapMemory},
    {"vkUnmapMemory", (PFN_vkUnmapMemory)vkUnmapMemory}, {"vkCreatePipelineLayout", (PFN_vkVoidFunction)vkCreatePipelineLayout},
    {"vkDestroyPipelineLayout", (PFN_vkDestroyPipelineLayout)vkDestroyPipelineLayout}, {"vkCreateGraphicsPipelines", (PFN_vkVoidFunction)vkCreateGraphicsPipelines},
    {"vkDestroyPipeline", (PFN_vkDestroyPipeline)vkDestroyPipeline},

    // [command.cpp]
    {"vkCreateCommandPool", (PFN_vkCreateCommandPool)vkCreateCommandPool}, {"vkDestroyCommandPool", (PFN_vkDestroyCommandPool)vkDestroyCommandPool},
    {"vkAllocateCommandBuffers", (PFN_vkAllocateCommandBuffers)vkAllocateCommandBuffers}, {"vkFreeCommandBuffers", (PFN_vkFreeCommandBuffers)vkFreeCommandBuffers},
    {"vkBeginCommandBuffer", (PFN_vkBeginCommandBuffer)vkBeginCommandBuffer}, {"vkEndCommandBuffer", (PFN_vkEndCommandBuffer)vkEndCommandBuffer},
    {"vkResetCommandBuffer", (PFN_vkResetCommandBuffer)vkResetCommandBuffer}, {"vkResetCommandPool", (PFN_vkResetCommandPool)vkResetCommandPool},
    {"vkQueueSubmit", (PFN_vkQueueSubmit)vkQueueSubmit}, {"vkCmdBeginRenderPass", (PFN_vkCmdBeginRenderPass)vkCmdBeginRenderPass},
    {"vkCmdEndRenderPass", (PFN_vkCmdEndRenderPass)vkCmdEndRenderPass}, {"vkCmdBindPipeline", (PFN_vkCmdBindPipeline)vkCmdBindPipeline},
    {"vkCmdBindVertexBuffers", (PFN_vkCmdBindVertexBuffers)vkCmdBindVertexBuffers}, {"vkCmdBindIndexBuffer", (PFN_vkCmdBindIndexBuffer)vkCmdBindIndexBuffer},
    {"vkCmdBindDescriptorSets", (PFN_vkCmdBindDescriptorSets)vkCmdBindDescriptorSets}, {"vkCmdSetViewport", (PFN_vkCmdSetViewport)vkCmdSetViewport},
    {"vkCmdSetScissor", (PFN_vkCmdSetScissor)vkCmdSetScissor}, {"vkCmdDraw", (PFN_vkCmdDraw)vkCmdDraw},
    {"vkCmdDrawIndexed", (PFN_vkCmdDrawIndexed)vkCmdDrawIndexed}, {"vkCmdDrawIndirect", (PFN_vkCmdDrawIndirect)vkCmdDrawIndirect},
    {"vkCmdDrawIndexedIndirect", (PFN_vkCmdDrawIndexedIndirect)vkCmdDrawIndexedIndirect},

    // [shader.cpp]
    {"vkCreateShaderModule", (PFN_vkVoidFunction)(PFN_vkCreateShaderModule)vkCreateShaderModule},
    {"vkDestroyShaderModule", (PFN_vkVoidFunction)(PFN_vkDestroyShaderModule)vkDestroyShaderModule},
    {"vkCreateDescriptorSet", (PFN_vkVoidFunction)(PFN_vkCreateDescriptorSet)vkCreateDescriptorSet},
    {"vkUpdateDescriptorSets", (PFN_vkVoidFunction)(PFN_vkUpdateDescriptorSets)vkUpdateDescriptorSets},
    {"vkCmdBindDescriptorSets", (PFN_vkVoidFunction)(PFN_vkCmdBindDescriptorSets)vkCmdBindDescriptorSets},

    // [screen.cpp]
    {"vkCreateSwapchainKHR", (PFN_vkVoidFunction)vkCreateSwapchainKHR}, {"vkDestroySwapchainKHR", (PFN_vkVoidFunction)vkDestroySwapchainKHR},
    {"vkGetSwapchainImagesKHR", (PFN_vkVoidFunction)vkGetSwapchainImagesKHR}, {"vkAcquireNextImageKHR", (PFN_vkVoidFunction)vkAcquireNextImageKHR},
    {"vkQueuePresentKHR", (PFN_vkVoidFunction)vkQueuePresentKHR},

    // [descriptor.cpp]
    {"vkCreateDescriptorSetLayout", (PFN_vkVoidFunction)vkCreateDescriptorSetLayout}, {"vkDestroyDescriptorSetLayout", (PFN_vkVoidFunction)vkDestroyDescriptorSetLayout},
    {"vkCreateDescriptorPool", (PFN_vkVoidFunction)vkCreateDescriptorPool}, {"vkDestroyDescriptorPool", (PFN_vkDestroyDescriptorPool)vkDestroyDescriptorPool},
    {"vkAllocateDescriptorSets", (PFN_vkVoidFunction)vkAllocateDescriptorSets}, {"vkFreeDescriptorSets", (PFN_vkVoidFunction)vkFreeDescriptorSets},

    // [vulkan_layer.cpp]
    {"vkCreateInstance", (PFN_vkCreateInstance)vkCreateInstance}, {"vkCreateDevice", (PFN_vkCreateDevice)vkCreateDevice},
    {"vkEnumeratePhysicalDevices", (PFN_vkEnumeratePhysicalDevices)vkEnumeratePhysicalDevices},
    {"vkGetPhysicalDeviceProperties", (PFN_vkGetPhysicalDeviceProperties)vkGetPhysicalDeviceProperties},
    {"vkGetPhysicalDeviceMemoryProperties", (PFN_vkGetPhysicalDeviceMemoryProperties)vkGetPhysicalDeviceMemoryProperties}

    // [context.cpp]
    {"vkCreateInstance", (PFN_vkVoidFunction)vkCreateInstance},
    {"vkCreateDevice", (PFN_vkVoidFunction)vkCreateDevice},
    {"vkEnumeratePhysicalDevices", (PFN_vkVoidFunction)vkEnumeratePhysicalDevices},
    {"vkGetPhysicalDeviceProperties", (PFN_vkVoidFunction)vkGetPhysicalDeviceProperties},
    {"vkGetPhysicalDeviceMemoryProperties", (PFN_vkVoidFunction)vkGetPhysicalDeviceMemoryProperties}
};

extern "C" PFN_vkVoidFunction get_vulkan_proc_addr(const char* pName) {
    for (auto& entry : dispatch_table) if (strcmp(pName, entry.name) == 0) return entry.func;
    return nullptr;
}
