#include "stpipeline.h"
#include "../manager/stmng.h"
#include <GLES3/gl31.h>
#include <cstdint>
#include <cstring>
#include <vector>
#include <new>

static uint32_t MapBlendFactor(int32_t vkFactor) {
    switch (vkFactor) {
        case 0: return GL_ZERO; case 1: return GL_ONE;
        case 2: return GL_SRC_COLOR; case 3: return GL_ONE_MINUS_SRC_COLOR;
        case 4: return GL_DST_COLOR; case 5: return GL_ONE_MINUS_DST_COLOR;
        case 6: return GL_SRC_ALPHA; case 7: return GL_ONE_MINUS_SRC_ALPHA;
        case 8: return GL_DST_ALPHA; case 9: return GL_ONE_MINUS_DST_ALPHA;
        case 10: return GL_CONSTANT_COLOR; case 11: return GL_ONE_MINUS_CONSTANT_COLOR;
        case 12: return GL_CONSTANT_ALPHA; case 13: return GL_ONE_MINUS_CONSTANT_ALPHA;
        case 14: return GL_SRC_ALPHA_SATURATE;
        default: return GL_ZERO;
    }
}

static uint32_t MapBlendOp(int32_t vkOp) {
    switch (vkOp) {
        case 0: return GL_FUNC_ADD;
        case 1: return GL_FUNC_SUBTRACT;
        case 2: return GL_FUNC_REVERSE_SUBTRACT;
        case 3: return GL_MIN;
        case 4: return GL_MAX;
        default: return GL_FUNC_ADD;
    }
}

extern "C" {

int32_t vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, int32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const void* pAllocator, VkPipeline* pPipeline) {
    if (!device || !pCreateInfos || !pPipeline) return -3;
    for (int32_t i = 0; i < createInfoCount; ++i) {
        const auto& info = pCreateInfos[i];
        auto* pipeline = new(std::nothrow) VkPipeline_T();
        if (!pipeline) return -3;
        pipeline->bindPoint = 0;
        pipeline->layout = info.layout;
        pipeline->program = glCreateProgram();
        std::vector<GLuint> shaders;
        for (int32_t j = 0; j < info.stageCount; ++j) {
            GLenum glStage = (info.pStages[j].stage == 0x00000001) ? GL_VERTEX_SHADER : (info.pStages[j].stage == 0x00000010) ? GL_FRAGMENT_SHADER : 0;
            if (!glStage) continue;
            GLuint shader = glCreateShader(glStage);
            const GLchar* source = info.pStages[j].module->glsl_source.c_str();
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled) {
                glDeleteShader(shader);
                for (GLuint s : shaders) glDeleteShader(s);
                glDeleteProgram(pipeline->program);
                delete pipeline;
                for (int32_t k = 0; k < i; ++k) {
                    if (pPipeline[k]) { glDeleteProgram(pPipeline[k]->program); delete pPipeline[k]; pPipeline[k] = nullptr; }
                }
                return -3;
            }
            glAttachShader(pipeline->program, shader);
            shaders.push_back(shader);
        }
        glLinkProgram(pipeline->program);
        GLint linked = 0;
        glGetProgramiv(pipeline->program, GL_LINK_STATUS, &linked);
        for (GLuint s : shaders) glDeleteShader(s);
        if (!linked) {
            glDeleteProgram(pipeline->program);
            delete pipeline;
            for (int32_t k = 0; k < i; ++k) {
                if (pPipeline[k]) { glDeleteProgram(pPipeline[k]->program); delete pPipeline[k]; pPipeline[k] = nullptr; }
            }
            return -3;
        }
        pipeline->topology = info.pInputAssemblyState ? (info.pInputAssemblyState->topology == 0 ? GL_POINTS : info.pInputAssemblyState->topology == 1 ? GL_LINES : info.pInputAssemblyState->topology == 2 ? GL_LINE_STRIP : info.pInputAssemblyState->topology == 3 ? GL_TRIANGLES : info.pInputAssemblyState->topology == 4 ? GL_TRIANGLE_STRIP : info.pInputAssemblyState->topology == 5 ? GL_TRIANGLE_FAN : GL_TRIANGLES) : GL_TRIANGLES;
        if (info.pRasterizationState) {
            pipeline->cullModeEnable = info.pRasterizationState->cullMode != 0;
            pipeline->cullFace = info.pRasterizationState->cullMode == 1 ? 0x0404 : info.pRasterizationState->cullMode == 2 ? 0x0405 : info.pRasterizationState->cullMode == 3 ? 0x0408 : 0;
            pipeline->frontFace = info.pRasterizationState->frontFace == 0 ? 0x0901 : 0x0900;
        } else pipeline->cullModeEnable = 0;
        if (info.pDepthStencilState) {
            pipeline->depthTestEnable = info.pDepthStencilState->depthTestEnable;
            pipeline->depthCompareOp = info.pDepthStencilState->depthCompareOp >= 0 && info.pDepthStencilState->depthCompareOp <= 7 ? 0x0200 + info.pDepthStencilState->depthCompareOp : 0x0207;
        } else pipeline->depthTestEnable = 0;
        if (info.pColorBlendState && info.pColorBlendState->attachmentCount > 0) {
            const auto& blend = info.pColorBlendState->pAttachments[0];
            pipeline->blendEnable = blend.blendEnable;
            pipeline->blendSrcFactor = MapBlendFactor(blend.srcColorBlendFactor);
            pipeline->blendDstFactor = MapBlendFactor(blend.dstColorBlendFactor);
            pipeline->blendEquation = MapBlendOp(blend.colorBlendOp);
        } else pipeline->blendEnable = 0;
        pPipeline[i] = pipeline;
    }
    return 0;
}
                      
int32_t vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, int32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const void* pAllocator, VkPipeline* pPipeline) {
    if (!device || !pCreateInfos || !pPipeline) return -3;
    for (int32_t i = 0; i < createInfoCount; i++) {
        auto* pipeline = new(std::nothrow) VkPipeline_T();
        if (!pipeline) return -3;
        pipeline->bindPoint = 1;
        pipeline->program = glCreateProgram();
        pipeline->layout = pCreateInfos[i].layout;
        GLuint s = glCreateShader(GL_COMPUTE_SHADER);
        const GLchar* src = pCreateInfos[i].stage.module->glsl_source.c_str();
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        GLint ok;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            glDeleteShader(s);
            glDeleteProgram(pipeline->program);
            delete pipeline;
            return -3;
        }
        glAttachShader(pipeline->program, s);
        glLinkProgram(pipeline->program);
        glDeleteShader(s);
        glGetProgramiv(pipeline->program, GL_LINK_STATUS, &ok);
        if (!ok) {
            glDeleteProgram(pipeline->program);
            delete pipeline;
            return -3;
        }
        pPipeline[i] = pipeline;
    }
    return 0;
}

void vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const void* pAllocator) {
    if (pipeline) {
        if (pipeline->program != 0) glDeleteProgram(pipeline->program);
        delete pipeline;
    }
}

int32_t vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const void* pAllocator, VkPipelineLayout* pPipelineLayout) {
    if (!device || !pCreateInfo || !pPipelineLayout) return -3;
    auto* layouts = new(std::nothrow) VkPipelineLayout_T();
    if (!layouts) return -3;
    std::memset(layouts, 0, sizeof(*layouts));
    layouts->layoutId = (int64_t)layouts;
    layouts->setLayoutCount = pCreateInfo->setLayoutCount;
    layouts->pushConstantRangeCount = pCreateInfo->pushConstantRangeCount;    
    if (layouts->setLayoutCount > 0 && pCreateInfo->pSetLayouts) {
        layouts->setLayouts = new(std::nothrow) VkDescriptorSetLayout[layouts->setLayoutCount];
        if (!layouts->setLayouts) { delete layouts; return -3; }
        for (int32_t i = 0; i < layouts->setLayoutCount; ++i) layouts->setLayouts[i] = pCreateInfo->pSetLayouts[i];
    }    
    if (layouts->pushConstantRangeCount > 0 && pCreateInfo->pPushConstantRanges) {
        layouts->pushConstantRanges = new(std::nothrow) VkPushConstantRange[layouts->pushConstantRangeCount];
        if (!layouts->pushConstantRanges) {
            delete[] layouts->setLayouts;
            delete layouts;
            return -3;
        }
        for (int32_t i = 0; i < layouts->pushConstantRangeCount; ++i) layouts->pushConstantRanges[i] = pCreateInfo->pPushConstantRanges[i];
    }    
    *pPipelineLayout = layouts;
    return 0;
}

void vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout layout, const void* pAllocator) {
    if (!layout) return;
    delete[] layout->setLayouts;
    delete[] layout->pushConstantRanges;
    delete layout;
}

int32_t vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const void* pAllocator, VkPipelineCache* pPipelineCache) {
    if (!device || !pPipeline) return -3;
    auto* cache = new(std::nothrow) VkPipelineCache_T();
    if (!cache) return -3;
    std::memset(cache, 0, sizeof(*cache));
    cache->cacheId = (int64_t)cache;
    cache->flags = pCreateInfo ? pCreateInfo->flags : 0;
    cache->header.headerSize = sizeof(VkPipelineCacheHeaderVersionOne);
    cache->header.headerVersion = VK_PIPELINE_CACHE_HEADER_VERSION_ONE;
    cache->header.vendorID = device->vendorID;
    cache->header.deviceID = device->deviceID;
    std::memcpy(cache->header.pipelineCacheUUID, device->pipelineCacheUUID, VK_UUID_SIZE);
    if (!pCreateInfo || !pCreateInfo->pInitialData || pCreateInfo->initialDataSize == 0) {
        *pipeline = cache;
        return 0;
    }
    if (pCreateInfo->initialDataSize < sizeof(VkPipelineCacheHeaderVersionOne)) {
        delete cache;
        return -3;
    }
    const auto* header = static_cast<const VkPipelineCacheHeaderVersionOne*>(pCreateInfo->pInitialData);
    if (header->headerSize != sizeof(VkPipelineCacheHeaderVersionOne) || header->headerVersion != VK_PIPELINE_CACHE_HEADER_VERSION_ONE) {
        delete cache;
        return -3;
    }
    if (header->vendorID != cache->header.vendorID || header->deviceID != cache->header.deviceID ||
        std::memcmp(header->pipelineCacheUUID, cache->header.pipelineCacheUUID, VK_UUID_SIZE) != 0) {
        *pipeline = cache;
        return 0;
    }
    const uint8_t* data = static_cast<const uint8_t*>(pCreateInfo->pInitialData) + sizeof(VkPipelineCacheHeaderVersionOne);
    size_t remain = pCreateInfo->initialDataSize - sizeof(VkPipelineCacheHeaderVersionOne);
    while (remain >= sizeof(uint64_t) * 2 && cache->entryCount < MAX_CACHE_ENTRIES) {
        uint64_t hash;
        uint64_t size;
        std::memcpy(&hash, data, sizeof(hash));
        data += sizeof(hash);
        std::memcpy(&size, data, sizeof(size));
        data += sizeof(size);
        remain -= sizeof(uint64_t) * 2;
        if (size > remain) break;
        auto* entry = &cache->entries[cache->entryCount];
        entry->hash = hash;
        entry->ovsize = size;
        size_t words = (size + 3) / 4;
        entry->ovdata = new(std::nothrow) uint32_t[words];
        if (!entry->ovdata) break;
        std::memcpy(entry->ovdata, data, size);
        cache->entryCount++;
        data += size;
        remain -= size;
    }
    *pPipelineCache = cache;
    return 0;
}

void vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const void* pAllocator) {
    if (!pipelineCache) return;
    for (int32_t i = 0; i < pipelineCache->entryCount; ++i) {
        delete[] pipelineCache->entries[i].ovdata;
        pipelineCache->entries[i].ovdata = nullptr;
        pipelineCache->entries[i].ovsize = 0;
    }
    delete pipelineCache;
}

int32_t vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t* pSize, void* pData) {
    if (!device || !pipelineCache || !pSize) return -3;
    size_t totalSize = sizeof(VkPipelineCacheHeaderVersionOne);
    for (int32_t i = 0; i < pipelineCache->entryCount; ++i) {
        totalSize += sizeof(uint64_t) * 2;
        totalSize += pipelineCache->entries[i].ovsize;
    }
    if (!pData) {
        *pSize = totalSize;
        return 0;
    }
    size_t capacity = *pSize;
    if (capacity < sizeof(VkPipelineCacheHeaderVersionOne)) {
        *pSize = 0;
        return 0;
    }
    uint8_t* out = static_cast<uint8_t*>(pData);
    std::memcpy(out, &pipelineCache->header, sizeof(VkPipelineCacheHeaderVersionOne));
    out += sizeof(VkPipelineCacheHeaderVersionOne);
    size_t written = sizeof(VkPipelineCacheHeaderVersionOne);
    for (int32_t i = 0; i < pipelineCache->entryCount; ++i) {
        const auto& entry = pipelineCache->entries[i];
        size_t required = sizeof(uint64_t) * 2 + entry.ovsize;
        if (written + required > capacity) {
            *pSize = written;
            return VK_INCOMPLETE;
        }
        std::memcpy(out, &entry.hash, sizeof(uint64_t));
        out += sizeof(uint64_t);
        std::memcpy(out, &entry.ovsize, sizeof(uint64_t));
        out += sizeof(uint64_t);
        std::memcpy(out, entry.ovdata, entry.ovsize);
        out += entry.ovsize;
        written += required;
    }
    *pSize = written;
    return 0;
}

int32_t vkMergePipelineCaches(VkDevice device, VkPipelineCache dst, int32_t srcCount, const VkPipelineCache* pSrc) {
    if (!device || !dst || !pSrc || srcCount <= 0) return -3;
    for (int32_t i = 0; i < srcCount; ++i) {
        VkPipelineCache src = pSrc[i];
        if (!src || src == dst) continue;
        for (int32_t j = 0; j < src->entryCount; ++j) {
            const auto& srcEntry = src->entries[j];
            bool found = false;
            for (int32_t k = 0; k < dst->entryCount; ++k) {
                if (dst->entries[k].hash == srcEntry.hash) {
                    found = true; break;
                }
            }
            if (found) continue;
            if (dst->entryCount >= MAX_CACHE_ENTRIES) continue;
            auto& dstEntry = dst->entries[dst->entryCount];
            dstEntry.hash = srcEntry.hash;
            dstEntry.ovsize = srcEntry.ovsize;
            dstEntry.ovdata = nullptr;
            if (srcEntry.ovsize > 0) {
                dstEntry.ovdata = new(std::nothrow)uint32_t[(srcEntry.ovsize + 3) / 4];
                if (!dstEntry.ovdata) {
                    dstEntry.ovsize = 0; continue;
                }
                std::memset(dstEntry.ovdata, 0, ((srcEntry.ovsize + 3) / 4) * sizeof(uint32_t));
                std::memcpy(dstEntry.ovdata, srcEntry.ovdata, srcEntry.ovsize);
            }
            dst->entryCount++;
        }
    }
    return 0;
}

}        
