#include <GLES3/gl31.h>
#include <stdlib.h>
#include <stdint.h>

typedef void* VkDevice;
typedef uint64_t VkBuffer;
typedef uint64_t VkDeviceMemory;
typedef uint64_t VkDeviceSize;
typedef uint64_t VkPipeline;
typedef uint64_t VkPipelineLayout;
typedef uint32_t VkResult;

#define VK_SUCCESS 0

struct FakeMemory {
    void* cpuData;
    VkDeviceSize size;
    GLuint boundBufferID;
};

extern "C" {

// --- Memory & Buffers ---
VkResult vkCreateBuffer(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkBuffer* pBuffer) {
    if (!pBuffer) return 1;    
    GLuint bufferID = 0;
    glGenBuffers(1, &bufferID);        
    BufferContext* ctx = (BufferContext*)malloc(sizeof(BufferContext));
    ctx->glBuffer = bufferID;
    ctx->size = 0; 
    *pBuffer = (VkBuffer)(uintptr_t)ctx;
    return VK_SUCCESS;
}

void vkDestroyBuffer(VkDevice device, VkBuffer buffer, const void* pAllocator) {
    if (buffer) {
        BufferContext* ctx = (BufferContext*)(uintptr_t)buffer;
        glDeleteBuffers(1, &ctx->glBuffer); 
        free(ctx);
    }
}

VkResult vkAllocateMemory(VkDevice device, const void* pAllocateInfo, const void* pAllocator, VkDeviceMemory* pMemory) {    
    VkDeviceSize size = *(VkDeviceSize*)((char*)pAllocateInfo + 16);    
    MemoryBlock* mem = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    mem->cpuPtr = malloc(size); 
    mem->size = size;
    mem->boundGlBuffer = 0;     
    
    *pMemory = (VkDeviceMemory)(uintptr_t)mem;
    return VK_SUCCESS;
}

void vkFreeMemory(VkDevice device, VkDeviceMemory memory, const void* pAllocator) {
    if (memory) {
        FakeMemory* mem = (FakeMemory*)memory;
        free(mem->cpuData);
        free(mem);
    }
}

VkResult vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) {    
    BufferContext* ctx = (BufferContext*)(uintptr_t)buffer;
    MemoryBlock* mem = (MemoryBlock*)memory;    
    mem->boundGlBuffer = ctx->glBuffer;
    return VK_SUCCESS;
}

VkResult vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, uint32_t flags, void** ppData) {    
    MemoryBlock* mem = (MemoryBlock*)memory;        
    if (ppData) {
        *ppData = (char*)mem->cpuPtr + offset;
    }
    
    return VK_SUCCESS;
}

void vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    MemoryBlock* mem = (MemoryBlock*)memory;        
    if (mem->boundGlBuffer != 0) {        
        glBindBuffer(GL_ARRAY_BUFFER, mem->boundGlBuffer);                
        glBufferData(GL_ARRAY_BUFFER, mem->size, mem->cpuPtr, GL_STATIC_DRAW);
    }
}

// --- Pipeline ---
VkResult vkCreatePipelineLayout(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkPipelineLayout* pPipelineLayout) {
    if (pPipelineLayout) *pPipelineLayout = (VkPipelineLayout)1;
    return VK_SUCCESS;
}

void vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout layout, const void* pAllocator) {}

VkResult vkCreateGraphicsPipelines(VkDevice device, uint64_t cache, uint32_t count, const VkGraphicsPipelineCreateInfo* pCreateInfos, const void* pAlloc, uint64_t* pPipelines) {
    for (uint32_t i = 0; i < count; ++i) {
        const VkGraphicsPipelineCreateInfo& info = pCreateInfos[i];
        GLuint pipeline;
        glGenProgramPipelines(1, &pipeline);
        for (uint32_t j = 0; j < info.stageCount; ++j) {
            const VkPipelineShaderStageCreateInfo& stage = info.pStages[j];
            GLuint shaderID = (GLuint)(uintptr_t)stage.module;
            GLbitfield stageBit = (stage.stage == VK_SHADER_STAGE_VERTEX_BIT) ? GL_VERTEX_SHADER_BIT : GL_FRAGMENT_SHADER_BIT;            
            glUseProgramStages(pipeline, stageBit, shaderID);
        }
        glValidateProgramPipeline(pipeline);
        pPipelines[i] = (uint64_t)pipeline;
    }
    return VK_SUCCESS;
}

void vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const void* pAllocator) {}

}
