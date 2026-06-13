#include <GLES3/gl31.h>
#include <stdint.h>

typedef void* VkDevice;
typedef void* VkCommandPool;
typedef void* VkCommandBuffer;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

struct FakeCmdAllocInfo {
    uint32_t sType;
    const void* pNext;
    VkCommandPool commandPool;
    uint32_t level;
    uint32_t commandBufferCount;
};

extern "C" {

VkResult vkCreateCommandPool(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkCommandPool* pCommandPool) {
    if (pCommandPool) *pCommandPool = (VkCommandPool)1;
    return VK_SUCCESS;
}

void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const void* pAllocator) {}

VkResult vkAllocateCommandBuffers(VkDevice device, const void* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {
    if (!pAllocateInfo || !pCommandBuffers) return VK_SUCCESS;
    const FakeCmdAllocInfo* info = (const FakeCmdAllocInfo*)pAllocateInfo;
    for (uint32_t i = 0; i < info->commandBufferCount; ++i) {
        pCommandBuffers[i] = (VkCommandBuffer)(uintptr_t)(i + 1);
    }
    return VK_SUCCESS;
}

void vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {}

VkResult vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const void* pBeginInfo) {
    return VK_SUCCESS;
}

VkResult vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    return VK_SUCCESS;
}

VkResult vkResetCommandBuffer(VkCommandBuffer commandBuffer, uint32_t flags) {
    return VK_SUCCESS;
}

VkResult vkResetCommandPool(VkDevice device, VkCommandPool commandPool, uint32_t flags) {
    return VK_SUCCESS;
}

}
