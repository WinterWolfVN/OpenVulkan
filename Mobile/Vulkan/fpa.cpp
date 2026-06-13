#include <string.h>

struct FPAEntry { const char* name; PFN_vkVoidFunction func; };

static const FPAEntry dispatch_table[] = {
    // [buffer.cpp]
    {"vkCreateBuffer", (PFN_vkVoidFunction)vkCreateBuffer}, 
    {"vkDestroyBuffer", (PFN_vkDestroyBuffer)vkDestroyBuffer},
    {"vkAllocateMemory", (PFN_vkAllocateMemory)vkAllocateMemory}, 
    {"vkFreeMemory", (PFN_vkFreeMemory)vkFreeMemory},
    {"vkBindBufferMemory", (PFN_vkBindBufferMemory)vkBindBufferMemory}, 
    {"vkMapMemory", (PFN_vkMapMemory)vkMapMemory},
    {"vkUnmapMemory", (PFN_vkUnmapMemory)vkUnmapMemory}, 
    {"vkCreatePipelineLayout", (PFN_vkVoidFunction)vkCreatePipelineLayout},
    {"vkDestroyPipelineLayout", (PFN_vkDestroyPipelineLayout)vkDestroyPipelineLayout}, 
    {"vkCreateGraphicsPipelines", (PFN_vkVoidFunction)vkCreateGraphicsPipelines},
    {"vkDestroyPipeline", (PFN_vkDestroyPipeline)vkDestroyPipeline},
    {"vkCmdBindPipeline", (PFN_vkVoidFunction)(PFN_vkCmdBindPipeline)vkCmdBindPipeline},

    // [command.cpp]
    {"vkCreateCommandPool", (PFN_vkVoidFunction)(PFN_vkCreateCommandPool)vkCreateCommandPool},
    {"vkDestroyCommandPool", (PFN_vkVoidFunction)(PFN_vkDestroyCommandPool)vkDestroyCommandPool},
    {"vkAllocateCommandBuffers", (PFN_vkVoidFunction)(PFN_vkAllocateCommandBuffers)vkAllocateCommandBuffers},
    {"vkFreeCommandBuffers", (PFN_vkVoidFunction)(PFN_vkFreeCommandBuffers)vkFreeCommandBuffers},
    {"vkBeginCommandBuffer", (PFN_vkVoidFunction)(PFN_vkBeginCommandBuffer)vkBeginCommandBuffer},
    {"vkEndCommandBuffer", (PFN_vkVoidFunction)(PFN_vkEndCommandBuffer)vkEndCommandBuffer},
    {"vkResetCommandBuffer", (PFN_vkVoidFunction)(PFN_vkResetCommandBuffer)vkResetCommandBuffer},
    {"vkResetCommandPool", (PFN_vkVoidFunction)(PFN_vkResetCommandPool)vkResetCommandPool},
    {"vkCmdBeginRenderPass", (PFN_vkVoidFunction)(PFN_vkCmdBeginRenderPass)vkCmdBeginRenderPass},
    {"vkCmdEndRenderPass", (PFN_vkVoidFunction)(PFN_vkCmdEndRenderPass)vkCmdEndRenderPass},    
    {"vkCmdBindVertexBuffers", (PFN_vkVoidFunction)(PFN_vkCmdBindVertexBuffers)vkCmdBindVertexBuffers},
    {"vkCmdBindIndexBuffer", (PFN_vkVoidFunction)(PFN_vkCmdBindIndexBuffer)vkCmdBindIndexBuffer},    
    {"vkCmdSetViewport", (PFN_vkVoidFunction)(PFN_vkCmdSetViewport)vkCmdSetViewport},
    {"vkCmdSetScissor", (PFN_vkVoidFunction)(PFN_vkCmdSetScissor)vkCmdSetScissor},
    {"vkCmdDraw", (PFN_vkVoidFunction)(PFN_vkCmdDraw)vkCmdDraw},
    {"vkCmdDrawIndexed", (PFN_vkVoidFunction)(PFN_vkCmdDrawIndexed)vkCmdDrawIndexed},
    {"vkCmdDrawIndirect", (PFN_vkVoidFunction)(PFN_vkCmdDrawIndirect)vkCmdDrawIndirect},
    {"vkCmdDrawIndexedIndirect", (PFN_vkVoidFunction)(PFN_vkCmdDrawIndexedIndirect)vkCmdDrawIndexedIndirect},
    {"vkQueueSubmit", (PFN_vkVoidFunction)(PFN_vkQueueSubmit)vkQueueSubmit},

    // [shader.cpp]
    {"vkCreateShaderModule", (PFN_vkVoidFunction)(PFN_vkCreateShaderModule)vkCreateShaderModule},
    {"vkDestroyShaderModule", (PFN_vkVoidFunction)(PFN_vkDestroyShaderModule)vkDestroyShaderModule},
    {"vkCreateDescriptorSetLayout", (PFN_vkVoidFunction)(PFN_vkCreateDescriptorSetLayout)vkCreateDescriptorSetLayout},
    {"vkDestroyDescriptorSetLayout", (PFN_vkVoidFunction)(PFN_vkDestroyDescriptorSetLayout)vkDestroyDescriptorSetLayout},
    {"vkCreateDescriptorPool", (PFN_vkVoidFunction)(PFN_vkCreateDescriptorPool)vkCreateDescriptorPool},
    {"vkDestroyDescriptorPool", (PFN_vkVoidFunction)(PFN_vkDestroyDescriptorPool)vkDestroyDescriptorPool},
    {"vkAllocateDescriptorSets", (PFN_vkVoidFunction)(PFN_vkAllocateDescriptorSets)vkAllocateDescriptorSets},
    {"vkFreeDescriptorSets", (PFN_vkVoidFunction)(PFN_vkFreeDescriptorSets)vkFreeDescriptorSets},
    {"vkUpdateDescriptorSets", (PFN_vkVoidFunction)(PFN_vkUpdateDescriptorSets)vkUpdateDescriptorSets},
    {"vkCmdBindDescriptorSets", (PFN_vkVoidFunction)(PFN_vkCmdBindDescriptorSets)vkCmdBindDescriptorSets},
    
    // [screen.cpp]
    {"vkCreateSwapchainKHR", (PFN_vkVoidFunction)vkCreateSwapchainKHR}, 
    {"vkDestroySwapchainKHR", (PFN_vkVoidFunction)vkDestroySwapchainKHR},
    {"vkGetSwapchainImagesKHR", (PFN_vkVoidFunction)vkGetSwapchainImagesKHR}, 
    {"vkAcquireNextImageKHR", (PFN_vkVoidFunction)vkAcquireNextImageKHR},
    {"vkQueuePresentKHR", (PFN_vkVoidFunction)vkQueuePresentKHR},
    
    // [vulkan_layer.cpp]
    {"vkCreateInstance", (PFN_vkCreateInstance)vkCreateInstance}, 
    {"vkCreateDevice", (PFN_vkCreateDevice)vkCreateDevice},
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
