#include "../struct/stmng.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <cstring>

extern "C" {

int32_t vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const void* pAllocator, VkDescriptorSetLayout* pSetLayout) {
    if (!device || !pCreateInfo || !pSetLayout) return -3;
    
    *pSetLayout = new VkDescriptorSetLayout_T();
    return 0;
}

void vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const void* pAllocator) {
    if (descriptorSetLayout) delete descriptorSetLayout;
}

int32_t vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const void* pAllocator, VkDescriptorPool* pDescriptorPool) {
    if (!device || !pCreateInfo || !pDescriptorPool) return -3;
    
    VkDescriptorPool_T* pool = new VkDescriptorPool_T();
    
    pool->maxSets = pCreateInfo->maxSets;
    pool->allocatedSets = 0;
    pool->maxUniformBuffers = 0;
    pool->maxCombinedImages = 0;

    for (uint32_t i = 0; i < pCreateInfo->poolSizeCount; ++i) {
        if (pCreateInfo->pPoolSizes[i].type == 6) {
            pool->maxUniformBuffers += pCreateInfo->pPoolSizes[i].descriptorCount;
        } else if (pCreateInfo->pPoolSizes[i].type == 1) {
            pool->maxCombinedImages += pCreateInfo->pPoolSizes[i].descriptorCount;
        }
    }

    *pDescriptorPool = pool;
    return 0;
}

void vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const void* pAllocator) {
    if (descriptorPool) delete descriptorPool;
}

int32_t vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) {
    if (!device || !pAllocateInfo || !pDescriptorSets) return -3;
    
    for (uint32_t i = 0; i < pAllocateInfo->descriptorSetCount; ++i) {
        VkDescriptorSet_T* set = new VkDescriptorSet_T();
        std::memset(set, 0, sizeof(VkDescriptorSet_T));
        set->setId = (int64_t)set;
        pDescriptorSets[i] = set;
    }
    return 0;
}

void vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) {
    if (!pDescriptorSets) return;
    for (uint32_t i = 0; i < descriptorSetCount; ++i) {
        if (pDescriptorSets[i]) delete pDescriptorSets[i];
    }
}

void vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const void* pDescriptorCopies) {
    if (!pDescriptorWrites) return;

    for (uint32_t i = 0; i < descriptorWriteCount; ++i) {
        const VkWriteDescriptorSet& write = pDescriptorWrites[i];
        VkDescriptorSet_T* set = (VkDescriptorSet_T*)write.dstSet;
        if (!set) continue;

        for (uint32_t j = 0; j < write.descriptorCount; ++j) {
            VkDescriptorBinding b;
            std::memset(&b, 0, sizeof(VkDescriptorBinding));
            
            b.binding = write.dstBinding + j;
            b.type = write.descriptorType;

            if (write.descriptorType == 6 || write.descriptorType == 7) { 
                if (write.pBufferInfo) {
                    b.bufferId = write.pBufferInfo[j].buffer->buffer;
                    b.offset = write.pBufferInfo[j].offset;
                    b.size = write.pBufferInfo[j].range;
                }
            } 
            else if (write.descriptorType == 1 || write.descriptorType == 2 || write.descriptorType == 3) { 
                if (write.pImageInfo) {
                    if (write.pImageInfo[j].imageView && write.pImageInfo[j].imageView->image) {
                        b.textureId = write.pImageInfo[j].imageView->image->texture;
                    }
                    if (write.pImageInfo[j].sampler) {
                        b.samplerId = write.pImageInfo[j].sampler->sampler;
                    }
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
            } 
            else if (set->bindingCount < MAX_BINDINGS) {
                set->bindings[set->bindingCount] = b;
                set->bindingCount++;
            }
        }
    }
}

}        
