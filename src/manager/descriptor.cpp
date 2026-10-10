#include "stmng.h"
#include <GLES3/gl31.h>
#include <cstring>
#include <new>

extern "C" {

static inline int32_t DT(int32_t t) { return (t == 1000138000) ? 11 : ((t >= 0 && t <= 10) ? t : 0); }

int32_t vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pLayoutCreateInfo, const void* pAllocator, VkDescriptorSetLayout* setLayout) {
    if (!device || !pLayoutCreateInfo || !setLayout) return -3;
    auto* layouts = new(std::nothrow) VkDescriptorSetLayout_T();
    if (!layouts) return -3;
    layouts->bindingCount = pLayoutCreateInfo->bindingCount;
    if (layouts->bindingCount > 0 && pLayoutCreateInfo->pBinding) {
        layouts->binding = new(std::nothrow) VkDescriptorSetLayoutBinding[layouts->bindingCount]();
        if (!layouts->binding) { delete layouts; return -3; }
        for (int32_t i = 0; i < layouts->bindingCount; ++i) {
            layouts->binding[i] = pLayoutCreateInfo->pBinding[i];
            if (pLayoutCreateInfo->pBinding[i].pImmutableSampler) {
                int32_t c = pLayoutCreateInfo->pBinding[i].descriptorCount;
                layouts->binding[i].pImmutableSampler = new(std::nothrow) VkSampler[c];
                if (layouts->binding[i].pImmutableSampler) 
                    std::memcpy((void*)layouts->binding[i].pImmutableSampler, pLayoutCreateInfo->pBinding[i].pImmutableSampler, c * sizeof(VkSampler));
            }
        }
    } else layouts->binding = nullptr;
    *setLayout = layouts; return 0;
}

void vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout setLayout, const void* pAllocator) {
    if (!setLayout) return;
    if (setLayout->binding) {
        for (int32_t i = 0; i < setLayout->bindingCount; ++i) delete[] setLayout->binding[i].pImmutableSampler;
        delete[] setLayout->binding;
    }
    delete setLayout;
}

int32_t vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pPoolCreateInfo, const void* pAllocator, VkDescriptorPool* pool) {
    if (!device || !pPoolCreateInfo || !pool) return -3;
    auto* pools = new(std::nothrow) VkDescriptorPool_T();
    if (!pools) return -3;
    std::memset(pools, 0, sizeof(*pools));
    pools->maxSets = pPoolCreateInfo->maxSets;
    if (pools->maxSets > 0) {
        pools->allocatedSetList = new(std::nothrow) VkDescriptorSet[pools->maxSets]();
        if (!pools->allocatedSetList) { delete pools; return -3; }
    }
    for (int32_t i = 0; i < pPoolCreateInfo->poolSizeCount; ++i) pools->maxDesc[DT(pPoolCreateInfo->pPoolSizes[i].type)] += pPoolCreateInfo->pPoolSizes[i].descriptorCount;
    *pool = pools; return 0;
}

void vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool pool, const void* pAllocator) {
    if (!pool) return;
    if (pool->allocatedSetList) {
        for (int32_t i = 0; i < pool->maxSets; ++i) delete pool->allocatedSetList[i];
        delete[] pool->allocatedSetList;
    }
    delete pool;
}

int32_t vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSet) {
    if (!device || !pAllocateInfo || !pDescriptorSet) return -3;
    auto* pools = (VkDescriptorPool_T*)pAllocateInfo->descriptorPool;
    if (!pools || pools->allocatedSets + pAllocateInfo->descriptorSetCount > pools->maxSets) return -3;
    int32_t r[12] = {0};
    for (int32_t i = 0; i < pAllocateInfo->descriptorSetCount; ++i) {
        auto* layouts = (VkDescriptorSetLayout_T*)pAllocateInfo->pSetLayout[i];
        if (layouts && layouts->binding)
            for (int32_t b = 0; b < layouts->bindingCount; ++b) r[DT(layouts->binding[b].descriptorType)] += layouts->binding[b].descriptorCount;
    }
    for (int32_t i = 0; i < 12; ++i) if (pools->allocDesc[i] + r[i] > pools->maxDesc[i]) return -3;
    for (int32_t i = 0; i < pAllocateInfo->descriptorSetCount; ++i) {
        auto* set = new(std::nothrow) VkDescriptorSet_T();
        if (!set) {
            for (int32_t k = 0; k < i; ++k) {
                for (int32_t j = 0; j < pools->maxSets; ++j) if (pools->allocatedSetList && pools->allocatedSetList[j] == pDescriptorSet[k]) pools->allocatedSetList[j] = nullptr;
                delete pDescriptorSet[k];
            }
            return -3;
        }
        std::memset(set, 0, sizeof(*set));
        set->setId = (int64_t)set;
        set->layout = (VkDescriptorSetLayout_T*)pAllocateInfo->pSetLayout[i];
        if (set->layout && set->layout->bindings) {
            for (int32_t b = 0; b < set->layout->bindingCount; ++b) {
                auto& bd = set->layout->bindings[b];
                if (!bd.pImmutableSampler) continue;
                for (int32_t j = 0; j < bd.descriptorCount && set->bindingCount < MAX_BINDINGS; ++j) {
                    auto& nb = set->bindings[set->bindingCount++];
                    nb.binding = bd.binding; nb.arrayElement = j; nb.type = bd.descriptorType;
                    if (bd.pImmutableSampler[j]) { nb.samplerId = bd.pImmutableSampler[j]->sampler; nb.isImmutableSampler = 1; }
                }
            }
        }
        pDescriptorSet[i] = set;
        for (int32_t j = 0; j < pools->maxSets; ++j) if (pools->allocatedSetList && !pools->allocatedSetList[j]) { pools->allocatedSetList[j] = set; break; }
    }
    for (int32_t i = 0; i < 12; ++i) pools->allocDesc[i] += r[i];
    pools->allocatedSets += pAllocateInfo->descriptorSetCount;
    return 0;
}

void vkFreeDescriptorSets(VkDevice device, VkDescriptorPool pool, int32_t descriptor, const VkDescriptorSet* pDescriptorSet) {
    if (!pDescriptorSet) return;
    auto* pools = (VkDescriptorPool_T*)pool;
    for (int32_t i = 0; i < descriptor; ++i) {
        if (!pDescriptorSet[i]) continue;
        auto* set = pDescriptorSet[i];
        if (pools && set->layout && set->layout->bindings) {
            for (int32_t b = 0; b < set->layout->bindingCount; ++b) pools->allocDesc[DT(set->layout->bindings[b].descriptorType)] -= set->layout->bindings[b].descriptorCount;
            for (int32_t j = 0; j < pools->maxSets; ++j) if (pools->allocatedSetList && pools->allocatedSetList[j] == set) pools->allocatedSetList[j] = nullptr;
        }
        delete set;
    }
    if (pools) {
        pools->allocatedSets = (descriptor > pools->allocatedSets) ? 0 : pools->allocatedSets - descriptor;
        for (int32_t i = 0; i < 12; ++i) if (pools->allocDesc[i] < 0) pools->allocDesc[i] = 0;
    }
}

void vkUpdateDescriptorSets(VkDevice device, int32_t descriptorWrite, const VkWriteDescriptorSet* pWriteDescriptorSet, int32_t descriptorCopy, const VkCopyDescriptorSet* pCopyDescriptorSet) {
    if (pWriteDescriptorSet) {
        for (int32_t i = 0; i < descriptorWrite; ++i) {
            const auto& w = pWriteDescriptorSet[i];
            auto* set = (VkDescriptorSet_T*)w.dstSet;
            if (!set || !set->layout || !set->layout->bindings) continue;
            bool ok = false;
            for (int32_t l = 0; l < set->layout->bindingCount; ++l)
                if (set->layout->bindings[l].binding == w.dstBinding && set->layout->bindings[l].descriptorType == w.descriptorType && w.dstArrayElement + w.descriptorCount <= set->layout->bindings[l].descriptorCount) { ok = true; break; }
            if (!ok) continue;
            for (int32_t j = 0; j < w.descriptorCount; ++j) {
                VkDescriptorBinding nb; std::memset(&nb, 0, sizeof(nb));
                nb.binding = w.dstBinding; nb.arrayElement = w.dstArrayElement + j; nb.type = w.descriptorType; nb.imageTarget = GL_TEXTURE_2D; nb.imageFormat = GL_RGBA8;
                int32_t idx = -1;
                for (int32_t k = 0; k < set->bindingCount; ++k) 
                    if (set->bindings[k].binding == nb.binding && set->bindings[k].arrayElement == nb.arrayElement) { idx = k; if (set->bindings[k].isImmutableSampler) { nb.samplerId = set->bindings[k].samplerId; nb.isImmutableSampler = 1; } break; }
                int t = w.descriptorType;
                if (t == 6 || t == 7 || t == 8 || t == 9) {
                    if (w.pBufferInfo && w.pBufferInfo[j].buffer) { nb.bufferId = w.pBufferInfo[j].buffer->buffer; nb.offset = w.pBufferInfo[j].offset; nb.size = w.pBufferInfo[j].range; }
                } else if (t == 1 || t == 2 || t == 3 || t == 10) {
                    if (w.pImageInfo) {
                        nb.imageLayout = w.pImageInfo[j].imageLayout;
                        if (w.pImageInfo[j].imageView) { if (w.pImageInfo[j].imageView->image) { nb.textureId = w.pImageInfo[j].imageView->image->texture; nb.imageFormat = w.pImageInfo[j].imageView->image->format; } if (w.pImageInfo[j].imageView->target) nb.imageTarget = w.pImageInfo[j].imageView->target; }
                        if (!nb.isImmutableSampler && w.pImageInfo[j].sampler) nb.samplerId = w.pImageInfo[j].sampler->sampler;
                    }
                } else if (t == 0) {
                    if (w.pImageInfo && w.pImageInfo[j].sampler && !nb.isImmutableSampler) nb.samplerId = w.pImageInfo[j].sampler->sampler;
                } else if (t == 4 || t == 5) {
                    if (w.pTexelBufferView) { nb.textureId = *(const uint32_t*)w.pTexelBufferView; nb.imageTarget = GL_TEXTURE_BUFFER; }
                }
                if (idx != -1) set->bindings[idx] = nb;
                else if (set->bindingCount < MAX_BINDINGS) set->bindings[set->bindingCount++] = nb;
            }
        }
    }
    if (pCopyDescriptorSet) {
        for (int32_t i = 0; i < descriptorCopy; ++i) {
            const auto& c = pCopyDescriptorSet[i];
            auto *src = (VkDescriptorSet_T*)c.srcSet, *dst = (VkDescriptorSet_T*)c.dstSet;
            if (!src || !dst) continue;
            for (int32_t j = 0; j < c.descriptorCount; ++j) {
                VkDescriptorBinding* sb = nullptr;
                for (int32_t k = 0; k < src->bindingCount; ++k) 
                    if (src->bindings[k].binding == c.srcBinding && src->bindings[k].arrayElement == c.srcArrayElement + j) { 
                       sb = &src->bindings[k];
                       break; 
                }
                if (!sb) continue;
                int32_t idx = -1;
                for (int32_t k = 0; k < dst->bindingCount; ++k) 
                    if (dst->bindings[k].binding == c.dstBinding && dst->bindings[k].arrayElement == c.dstArrayElement + j) { 
                       idx = k; 
                       break; 
                    }
                if (idx != -1) { 
                   dst->bindings[idx] = *sb; dst->bindings[idx].binding = c.dstBinding; dst->bindings[idx].arrayElement = c.dstArrayElement + j; }
                else if (dst->bindingCount < MAX_BINDINGS) { dst->bindings[dst->bindingCount] = *sb; dst->bindings[dst->bindingCount].binding = c.dstBinding; dst->bindings[dst->bindingCount++].arrayElement = c.dstArrayElement + j; }
            }
        }
    }
}

}        
