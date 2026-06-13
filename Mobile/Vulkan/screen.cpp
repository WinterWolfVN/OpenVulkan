#include <GLES3/gl31.h>
#include <stdint.h>

typedef void* VkDevice;
typedef uint64_t VkSwapchainKHR;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

extern "C" {

VkResult vkCreateSwapchainKHR(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkSwapchainKHR* pSwapchain) {
    if (pSwapchain) *pSwapchain = (VkSwapchainKHR)1;
    return VK_SUCCESS;
}

void vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const void* pAllocator) {}

VkResult vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pCount, void* pImages) {
    if (pCount) *pCount = 1;
    return VK_SUCCESS;
}

VkResult vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, void* semaphore, void* fence, uint32_t* pImageIndex) {
    if (pImageIndex) *pImageIndex = 0;
    return VK_SUCCESS;
}

VkResult vkQueuePresentKHR(void* queue, const void* pPresentInfo) {    
    return VK_SUCCESS;
}

}
