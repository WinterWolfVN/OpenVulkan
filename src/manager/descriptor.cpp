#include "../struct/stmng.h"
#include <GLES3/gl31.h>
#include <cstring>
#include <new>

extern "C" {

static inline int32_t DT(int32_t t) { return (t == 1000138000) ? 11 : ((t >= 0 && t <= 10) ? t : 0); }

int32_t vkCreateDescriptorSetLayout(VkDevice d, const VkDescriptorSetLayoutCreateInfo* ci, const void* a, VkDescriptorSetLayout* l) {
    if (!d || !ci || !l) return -3;
    auto* ly = new(std::nothrow) VkDescriptorSetLayout_T();
    if (!ly) return -3;
    ly->bindingCount = ci->bindingCount;
    if (ly->bindingCount > 0 && ci->pBindings) {
        ly->bindings = new(std::nothrow) VkDescriptorSetLayoutBinding[ly->bindingCount]();
        if (!ly->bindings) { delete ly; return -3; }
        for (int32_t i = 0; i < ly->bindingCount; ++i) {
            ly->bindings[i] = ci->pBindings[i];
            if (ci->pBindings[i].pImmutableSamplers) {
                int32_t c = ci->pBindings[i].descriptorCount;
                ly->bindings[i].pImmutableSamplers = new(std::nothrow) VkSampler[c];
                if (ly->bindings[i].pImmutableSamplers) 
                    std::memcpy((void*)ly->bindings[i].pImmutableSamplers, ci->pBindings[i].pImmutableSamplers, c * sizeof(VkSampler));
            }
        }
    } else ly->bindings = nullptr;
    *l = ly; return 0;
}

void vkDestroyDescriptorSetLayout(VkDevice d, VkDescriptorSetLayout l, const void* a) {
    if (!l) return;
    if (l->bindings) {
        for (int32_t i = 0; i < l->bindingCount; ++i) delete[] l->bindings[i].pImmutableSamplers;
        delete[] l->bindings;
    }
    delete l;
}

int32_t vkCreateDescriptorPool(VkDevice d, const VkDescriptorPoolCreateInfo* ci, const void* a, VkDescriptorPool* p) {
    if (!d || !ci || !p) return -3;
    auto* pl = new(std::nothrow) VkDescriptorPool_T();
    if (!pl) return -3;
    std::memset(pl, 0, sizeof(*pl));
    pl->maxSets = ci->maxSets;
    if (pl->maxSets > 0) {
        pl->allocatedSetList = new(std::nothrow) VkDescriptorSet[pl->maxSets]();
        if (!pl->allocatedSetList) { delete pl; return -3; }
    }
    for (int32_t i = 0; i < ci->poolSizeCount; ++i) pl->maxDesc[DT(ci->pPoolSizes[i].type)] += ci->pPoolSizes[i].descriptorCount;
    *p = pl; return 0;
}

void vkDestroyDescriptorPool(VkDevice d, VkDescriptorPool p, const void* a) {
    if (!p) return;
    if (p->allocatedSetList) {
        for (int32_t i = 0; i < p->maxSets; ++i) delete p->allocatedSetList[i];
        delete[] p->allocatedSetList;
    }
    delete p;
}

int32_t vkAllocateDescriptorSets(VkDevice d, const VkDescriptorSetAllocateInfo* ai, VkDescriptorSet* ds) {
    if (!d || !ai || !ds) return -3;
    auto* pl = (VkDescriptorPool_T*)ai->descriptorPool;
    if (!pl || pl->allocatedSets + ai->descriptorSetCount > pl->maxSets) return -3;
    int32_t r[12] = {0};
    for (int32_t i = 0; i < ai->descriptorSetCount; ++i) {
        auto* ly = (VkDescriptorSetLayout_T*)ai->pSetLayouts[i];
        if (ly && ly->bindings)
            for (int32_t b = 0; b < ly->bindingCount; ++b) r[DT(ly->bindings[b].descriptorType)] += ly->bindings[b].descriptorCount;
    }
    for (int32_t i = 0; i < 12; ++i) if (pl->allocDesc[i] + r[i] > pl->maxDesc[i]) return -3;
    for (int32_t i = 0; i < ai->descriptorSetCount; ++i) {
        auto* set = new(std::nothrow) VkDescriptorSet_T();
        if (!set) {
            for (int32_t k = 0; k < i; ++k) {
                for (int32_t j = 0; j < pl->maxSets; ++j) if (pl->allocatedSetList && pl->allocatedSetList[j] == ds[k]) pl->allocatedSetList[j] = nullptr;
                delete ds[k];
            }
            return -3;
        }
        std::memset(set, 0, sizeof(*set));
        set->setId = (int64_t)set;
        set->layout = (VkDescriptorSetLayout_T*)ai->pSetLayouts[i];
        if (set->layout && set->layout->bindings) {
            for (int32_t b = 0; b < set->layout->bindingCount; ++b) {
                auto& bd = set->layout->bindings[b];
                if (!bd.pImmutableSamplers) continue;
                for (int32_t j = 0; j < bd.descriptorCount && set->bindingCount < MAX_BINDINGS; ++j) {
                    auto& nb = set->bindings[set->bindingCount++];
                    nb.binding = bd.binding; nb.arrayElement = j; nb.type = bd.descriptorType;
                    if (bd.pImmutableSamplers[j]) { nb.samplerId = bd.pImmutableSamplers[j]->sampler; nb.isImmutableSampler = 1; }
                }
            }
        }
        ds[i] = set;
        for (int32_t j = 0; j < pl->maxSets; ++j) if (pl->allocatedSetList && !pl->allocatedSetList[j]) { pl->allocatedSetList[j] = set; break; }
    }
    for (int32_t i = 0; i < 12; ++i) pl->allocDesc[i] += r[i];
    pl->allocatedSets += ai->descriptorSetCount;
    return 0;
}

void vkFreeDescriptorSets(VkDevice d, VkDescriptorPool p, int32_t dc, const VkDescriptorSet* ds) {
    if (!ds) return;
    auto* pl = (VkDescriptorPool_T*)p;
    for (int32_t i = 0; i < dc; ++i) {
        if (!ds[i]) continue;
        auto* set = ds[i];
        if (pl && set->layout && set->layout->bindings) {
            for (int32_t b = 0; b < set->layout->bindingCount; ++b) pl->allocDesc[DT(set->layout->bindings[b].descriptorType)] -= set->layout->bindings[b].descriptorCount;
            for (int32_t j = 0; j < pl->maxSets; ++j) if (pl->allocatedSetList && pl->allocatedSetList[j] == set) pl->allocatedSetList[j] = nullptr;
        }
        delete set;
    }
    if (pl) {
        pl->allocatedSets = (dc > pl->allocatedSets) ? 0 : pl->allocatedSets - dc;
        for (int32_t i = 0; i < 12; ++i) if (pl->allocDesc[i] < 0) pl->allocDesc[i] = 0;
    }
}

void vkUpdateDescriptorSets(VkDevice d, int32_t dwc, const VkWriteDescriptorSet* dw, int32_t dcc, const VkCopyDescriptorSet* dcp) {
    if (dw) {
        for (int32_t i = 0; i < dwc; ++i) {
            const auto& w = dw[i];
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
                for (int32_t k = 0; k < set->bindingCount; ++k) if (set->bindings[k].binding == nb.binding && set->bindings[k].arrayElement == nb.arrayElement) { idx = k; if (set->bindings[k].isImmutableSampler) { nb.samplerId = set->bindings[k].samplerId; nb.isImmutableSampler = 1; } break; }
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
    if (dcp) {
        for (int32_t i = 0; i < dcc; ++i) {
            const auto& c = dcp[i];
            auto *src = (VkDescriptorSet_T*)c.srcSet, *dst = (VkDescriptorSet_T*)c.dstSet;
            if (!src || !dst) continue;
            for (int32_t j = 0; j < c.descriptorCount; ++j) {
                VkDescriptorBinding* sb = nullptr;
                for (int32_t k = 0; k < src->bindingCount; ++k) if (src->bindings[k].binding == c.srcBinding && src->bindings[k].arrayElement == c.srcArrayElement + j) { sb = &src->bindings[k]; break; }
                if (!sb) continue;
                int32_t idx = -1;
                for (int32_t k = 0; k < dst->bindingCount; ++k) if (dst->bindings[k].binding == c.dstBinding && dst->bindings[k].arrayElement == c.dstArrayElement + j) { idx = k; break; }
                if (idx != -1) { dst->bindings[idx] = *sb; dst->bindings[idx].binding = c.dstBinding; dst->bindings[idx].arrayElement = c.dstArrayElement + j; }
                else if (dst->bindingCount < MAX_BINDINGS) { dst->bindings[dst->bindingCount] = *sb; dst->bindings[dst->bindingCount].binding = c.dstBinding; dst->bindings[dst->bindingCount++].arrayElement = c.dstArrayElement + j; }
            }
        }
    }
}

}        
