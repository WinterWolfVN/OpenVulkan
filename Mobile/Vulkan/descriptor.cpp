#include <stdint.h>

typedef void* VkDevice;
typedef uint64_t VkDescriptorSetLayout;
typedef uint64_t VkDescriptorPool;
typedef uint64_t VkDescriptorSet;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

extern "C" {

VkResult vkCreateDescriptorSetLayout(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkDescriptorSetLayout* pSetLayout) {
    if (pSetLayout) *pSetLayout = (VkDescriptorSetLayout)1;
    return VK_SUCCESS;
}

void vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout, const void* pAllocator) {}

VkResult vkCreateDescriptorPool(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkDescriptorPool* pDescriptorPool) {
    if (pDescriptorPool) *pDescriptorPool = (VkDescriptorPool)1;
    return VK_SUCCESS;
}

void vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool pool, const void* pAllocator) {}

VkResult vkAllocateDescriptorSets(VkDevice device, const void* pAllocateInfo, VkDescriptorSet* pDescriptorSets) {    
    return VK_SUCCESS;
}

void vkFreeDescriptorSets(VkDevice device, VkDescriptorPool pool, uint32_t count, const VkDescriptorSet* pSets) {}

void vkUpdateDescriptorSets(VkDevice device, uint32_t writeCount, const void* pDescriptorWrites, uint32_t copyCount, const void* pDescriptorCopies) {    
}

}
