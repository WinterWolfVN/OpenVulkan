#include "../struct/stmng.h"
#include <GLES3/gl31.h>
#include <cstdint>

extern "C" {

int32_t vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const void* pAllocator, VkDescriptorSetLayout* pSetLayout) {
    if (!device || !pSetLayout) return -3;
    *pSetLayout = new VkDescriptorSetLayout_T();
    return 0;
}

void vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const void* pAllocator) {
    if (descriptorSetLayout) delete descriptorSetLayout;
}

int32_t vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const void* pAllocator, VkDescriptorPool* pDescriptorPool) {
    if (!device || !pDescriptorPool) return -3;
    *pDescriptorPool = new VkDescriptorPool_T();
    return 0;
}

void vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const void* pAllocator) {
    if (descriptorPool) delete descriptorPool;
}

int32_t vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) {
    if (!device || !pAllocateInfo || !pDescriptorSets) return -3;
    
    for (int32_t i = 0; i < pAllocateInfo->descriptorSetCount; ++i) {
        VkDescriptorSet set = new VkDescriptorSet_T();
        set->bindingCount = 0;
        pDescriptorSets[i] = set;
    }
    return 0;
}

void vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, int32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) {
    if (!pDescriptorSets) return;
    for (int32_t i = 0; i < descriptorSetCount; ++i) {
        if (pDescriptorSets[i]) delete pDescriptorSets[i];
    }
}

void vkUpdateDescriptorSets(VkDevice device, int32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, int32_t descriptorCopyCount, const void* pDescriptorCopies) {
    if (!pDescriptorWrites) return;

    for (int32_t i = 0; i < descriptorWriteCount; ++i) {
        const VkWriteDescriptorSet& write = pDescriptorWrites[i];
        VkDescriptorSet set = write.dstSet;
        if (!set) continue;

        for (int32_t j = 0; j < write.descriptorCount; ++j) {
            VkDescriptorBinding b;
            b.offset = 0;
            b.size = 0;
            b.binding = write.dstBinding + j;
            b.type = write.descriptorType;
            b.bufferId = 0;
            b.textureId = 0;
            b.samplerId = 0;
            b.padding = 0;

            if ((write.descriptorType == 6 || write.descriptorType == 7) && write.pBufferInfo) {
                b.bufferId = write.pBufferInfo[j].buffer->buffer;
                b.offset = write.pBufferInfo[j].offset;
                b.size = write.pBufferInfo[j].range;
            } 
            else if ((write.descriptorType == 1 || write.descriptorType == 3) && write.pImageInfo) {
                if (write.pImageInfo[j].imageView && write.pImageInfo[j].imageView->image) {
                    b.textureId = write.pImageInfo[j].imageView->image->texture;
                }
                if (write.pImageInfo[j].sampler) {
                    b.samplerId = write.pImageInfo[j].sampler->sampler;
                }
            }
            
            int32_t foundIdx = -1;
            for (int32_t k = 0; k < set->bindingCount; ++k) {
                if (set->bindings[k].binding == b.binding) {
                    foundIdx = k;
                    break;
                }
            }
            
            if (foundIdx != -1) {
                set->bindings[foundIdx] = b;
            } else if (set->bindingCount < 16) {
                set->bindings[set->bindingCount] = b;
                set->bindingCount++;
            }
        }
    }
}

}        
