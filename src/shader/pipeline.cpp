#include "../struct/stshader.h"
#include "../struct/stmng.h"
#include <GLES3/gl31.h>
#include <cstdint>
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

int32_t vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, int32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const void* pAllocator, VkPipeline* pPipelines) {
    if (!device || !pCreateInfos || !pPipelines) return -3;
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
            const GLchar* source = static_cast<const GLchar*>(info.pStages[j].module->code);
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
                    if (pPipelines[k]) { glDeleteProgram(pPipelines[k]->program); delete pPipelines[k]; pPipelines[k] = nullptr; }
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
                if (pPipelines[k]) { glDeleteProgram(pPipelines[k]->program); delete pPipelines[k]; pPipelines[k] = nullptr; }
            }
            return -3;
        }
        pipeline->topology = info.pInputAssemblyState ? (info.pInputAssemblyState->topology == 0 ? 0x0000 : info.pInputAssemblyState->topology == 1 ? 0x0001 : info.pInputAssemblyState->topology == 2 ? 0x0002 : info.pInputAssemblyState->topology == 3 ? 0x0004 : info.pInputAssemblyState->topology == 4 ? 0x0005 : 0x0004) : 0x0004;
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
        pPipelines[i] = pipeline;
    }
    return 0;
}
                      
int32_t vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, int32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const void* pAllocator, VkPipeline* pPipelines) {
    if (!device || !pCreateInfos || !pPipelines) return -3;
    for (int32_t i = 0; i < createInfoCount; ++i) {
        VkPipeline pipeline = new VkPipeline_T();
        pipeline->bindPoint = 1;
        pipeline->program = glCreateProgram();
        GLuint shader = glCreateShader(0x91B9);
        const GLchar* source = static_cast<const GLchar*>(pCreateInfos[i].stage.module->code);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);        
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            glDeleteShader(shader);
            glDeleteProgram(pipeline->program);
            delete pipeline;
            return -3;
        }
        glAttachShader(pipeline->program, shader);
        glLinkProgram(pipeline->program);        
        GLint linked = 0;
        glGetProgramiv(pipeline->program, GL_LINK_STATUS, &linked);        
        glDeleteShader(shader);
        if (!linked) {
            glDeleteProgram(pipeline->program);
            delete pipeline;
            return -3;
        }
        pPipelines[i] = pipeline;
    }
    return 0;
}

void vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const void* pAllocator) {
    if (pipeline) {
        if (pipeline->program != 0) glDeleteProgram(pipeline->program);
        delete pipeline;
    }
}

int32_t vkCreatePipelineLayout(VkDevice d, const VkPipelineLayoutCreateInfo* ci, const void* a, VkPipelineLayout* p) {
    if (!d || !ci || !p) return -3;
    auto* ly = new(std::nothrow) VkPipelineLayout_T();
    if (!ly) return -3;
    std::memset(ly, 0, sizeof(*ly));
    ly->layoutId = (int64_t)ly;
    ly->setLayoutCount = ci->setLayoutCount;
    ly->pushConstantRangeCount = ci->pushConstantRangeCount;    
    if (ly->setLayoutCount > 0 && ci->pSetLayouts) {
        ly->setLayouts = new(std::nothrow) VkDescriptorSetLayout[ly->setLayoutCount];
        if (!ly->setLayouts) { delete ly; return -3; }
        for (int32_t i = 0; i < ly->setLayoutCount; ++i) ly->setLayouts[i] = ci->pSetLayouts[i];
    }    
    if (ly->pushConstantRangeCount > 0 && ci->pPushConstantRanges) {
        ly->pushConstantRanges = new(std::nothrow) VkPushConstantRange[ly->pushConstantRangeCount];
        if (!ly->pushConstantRanges) {
            delete[] ly->setLayouts;
            delete ly;
            return -3;
        }
        for (int32_t i = 0; i < ly->pushConstantRangeCount; ++i) ly->pushConstantRanges[i] = ci->pPushConstantRanges[i];
    }    
    *p = ly;
    return 0;
}

void vkDestroyPipelineLayout(VkDevice d, VkPipelineLayout l, const void* a) {
    if (!l) return;
    delete[] l->setLayouts;
    delete[] l->pushConstantRanges;
    delete l;
}

int32_t vkCreatePipelineCache(VkDevice d, const VkPipelineCacheCreateInfo* ci, const void* a, VkPipelineCache* p) {
    if (!d || !p) return -3;
    auto* cache = new(std::nothrow) VkPipelineCache_T();
    if (!cache) return -3;
    std::memset(cache, 0, sizeof(*cache));
    cache->cacheId = (int64_t)cache;
    cache->entryCount = 0;
    cache->flags = ci ? ci->flags : 0;    
    if (ci && ci->pInitialData && ci->initialDataSize > 0) {
        const VkPipelineCacheHeader* header = (const VkPipelineCacheHeader*)ci->pInitialData;
        if (ci->initialDataSize >= sizeof(VkPipelineCacheHeader)) {
            const uint8_t* data = (const uint8_t*)ci->pInitialData + header->headerSize;
            uint64_t remaining = ci->initialDataSize - header->headerSize;            
            while (remaining >= sizeof(uint64_t) * 2) {
                uint64_t hash = *(const uint64_t*)data;
                uint64_t size = *(const uint64_t*)(data + sizeof(uint64_t));
                data += sizeof(uint64_t) * 2;
                remaining -= sizeof(uint64_t) * 2;             
                if (remaining >= size && cache->entryCount < MAX_CACHE_ENTRIES) {
                    auto& entry = cache->entries[cache->entryCount];
                    entry.hash = hash;
                    entry.binarySize = size;
                    entry.binaryData = new(std::nothrow) uint32_t[size / sizeof(uint32_t)];
                    if (entry.binaryData) {
                        std::memcpy(entry.binaryData, data, size);
                        cache->entryCount++;
                    }
                    data += size;
                    remaining -= size;
                } else {
                    break;
                }
            }
        }
    }    
    *p = cache;
    return 0;
}

void vkDestroyPipelineCache(VkDevice d, VkPipelineCache c, const void* a) {
    if (!c) return;
    for (int32_t i = 0; i < c->entryCount; ++i) {
        delete[] c->entries[i].binaryData;
    }
    delete c;
}

int32_t vkGetPipelineCacheData(VkDevice d, VkPipelineCache c, uint64_t* pSize, void* pData) {
    if (!d || !c || !pSize) return -3;    
    uint64_t totalSize = sizeof(VkPipelineCacheHeader);
    for (int32_t i = 0; i < c->entryCount; ++i) {
        totalSize += sizeof(uint64_t) * 2 + c->entries[i].binarySize;
    }    
    if (!pData) {
        *pSize = totalSize;
        return 0;
    }    
    if (*pSize < totalSize) return -3;    
    VkPipelineCacheHeader header;
    header.headerSize = sizeof(VkPipelineCacheHeader);
    header.headerVersion = 1;
    header.vendorID = 0;
    header.deviceID = 0;
    header.pipelineCacheUUID[0] = 0x4F564B00;
    header.pipelineCacheUUID[1] = 0x43414348;
    header.pipelineCacheUUID[2] = 0x45563031;
    header.pipelineCacheUUID[3] = 0x00000000;    
    std::memcpy(pData, &header, sizeof(header));
    uint8_t* ptr = (uint8_t*)pData + sizeof(header);    
    for (int32_t i = 0; i < c->entryCount; ++i) {
        *(uint64_t*)ptr = c->entries[i].hash;
        ptr += sizeof(uint64_t);
        *(uint64_t*)ptr = c->entries[i].binarySize;
        ptr += sizeof(uint64_t);
        std::memcpy(ptr, c->entries[i].binaryData, c->entries[i].binarySize);
        ptr += c->entries[i].binarySize;
    }    
    *pSize = totalSize;
    return 0;
}

int32_t vkMergePipelineCaches(VkDevice d, VkPipelineCache dst, int32_t srcCount, const VkPipelineCache* pSrc) {
    if (!d || !dst || !pSrc) return -3;    
    for (int32_t i = 0; i < srcCount; ++i) {
        VkPipelineCache src = pSrc[i];
        if (!src) continue;        
        for (int32_t j = 0; j < src->entryCount; ++j) {
            bool found = false;
            for (int32_t k = 0; k < dst->entryCount; ++k) {
                if (dst->entries[k].hash == src->entries[j].hash) {
                    found = true;
                    break;
                }
            }            
            if (!found && dst->entryCount < MAX_CACHE_ENTRIES) {
                auto& entry = dst->entries[dst->entryCount];
                entry.hash = src->entries[j].hash;
                entry.binarySize = src->entries[j].binarySize;
                entry.binaryData = new(std::nothrow) uint32_t[entry.binarySize / sizeof(uint32_t)];
                if (entry.binaryData) {
                    std::memcpy(entry.binaryData, src->entries[j].binaryData, entry.binarySize);
                    dst->entryCount++;
                }
            }
        }
    }    
    return 0;
}

}        
