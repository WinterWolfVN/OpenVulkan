#include <GLES3/gl31.h>
#include <dlfcn.h>
#include <vector>
#include <map>
#include <string.h>

#ifndef GL_SHADER_BINARY_FORMAT_SPIR_V
#define GL_SHADER_BINARY_FORMAT_SPIR_V 0x8F67
#endif

typedef void (GL_APIENTRYP PFNGLSPECIALIZESHADERPROC)(GLuint, const GLchar*, GLuint, const GLuint*, const GLuint*);
static PFNGLSPECIALIZESHADERPROC glSpecializeShader = nullptr;

typedef void* VkDevice;
typedef uint32_t VkResult;
typedef uint64_t VkShaderModule;
typedef uint64_t VkDescriptorSetLayout;
typedef uint64_t VkDescriptorPool;
typedef uint64_t VkDescriptorSet;

struct DescriptorInfo { uint32_t set; uint32_t binding; GLenum type; };
struct ShaderModuleContext { GLuint shaderID; std::vector<DescriptorInfo> descriptors; };

struct VkDescriptorSetLayout_T {
    std::vector<DescriptorInfo> bindings;
};

struct VkDescriptorPool_T {
    uint32_t maxSets;
};

struct DescriptorResource { GLenum type; GLuint id; };
struct VkDescriptorSet_T {
    std::map<uint32_t, DescriptorResource> resources;
};

extern "C" {

// --- Shader --- 
VkResult vkCreateShaderModule(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkShaderModule* pShaderModule) {
    if (!glSpecializeShader) {
        glSpecializeShader = (PFNGLSPECIALIZESHADERPROC)dlsym(RTLD_DEFAULT, "glSpecializeShader");
    }
    const uint32_t* raw = (const uint32_t*)pCreateInfo;
    size_t codeSize = *(size_t*)(raw + 4);
    const void* pCode = *(const void**)(raw + 6);
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, pCode, (GLsizei)codeSize);
    if (glSpecializeShader) {
        glSpecializeShader(shader, "main", 0, nullptr, nullptr);
    }
    *pShaderModule = (VkShaderModule)(uintptr_t)shader;
    return 0;
}

void vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const void* pAllocator) {
    ShaderModuleContext* ctx = (ShaderModuleContext*)(uintptr_t)shaderModule;
    glDeleteShader(ctx->shaderID);
    delete ctx;
}

// --- Descriptor Sets & Pipeline --- 
VkResult vkCreateDescriptorSetLayout(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkDescriptorSetLayout* pSetLayout) {
    auto* info = (const VkDescriptorSetLayoutCreateInfo*)pCreateInfo;
    VkDescriptorSetLayout_T* layout = new VkDescriptorSetLayout_T();
    for (uint32_t i = 0; i < info->bindingCount; ++i) {
        const auto& b = info->pBindings[i];
        GLenum glType = (b.descriptorType == 6) ? GL_UNIFORM_BUFFER : GL_SAMPLER_2D; 
        layout->bindings.push_back({0, b.binding, glType});
    }
    *pSetLayout = (VkDescriptorSetLayout)(uintptr_t)layout;
    return 0;
}

void vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const void* pAllocator) {
    delete (VkDescriptorSetLayout_T*)(uintptr_t)descriptorSetLayout;
}

VkResult vkCreateDescriptorPool(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkDescriptorPool* pDescriptorPool) {
    auto* info = (const VkDescriptorPoolCreateInfo*)pCreateInfo;
    VkDescriptorPool_T* pool = new VkDescriptorPool_T();
    pool->maxSets = info->maxSets;
    *pDescriptorPool = (VkDescriptorPool)(uintptr_t)pool;
    return 0;
}

void vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const void* pAllocator) {
    delete (VkDescriptorPool_T*)(uintptr_t)descriptorPool;
}

VkResult vkAllocateDescriptorSets(VkDevice device, const void* pAllocateInfo, VkDescriptorSet* pDescriptorSets) {
    auto* info = (const VkDescriptorSetAllocateInfo*)pAllocateInfo;
    for (uint32_t i = 0; i < info->descriptorSetCount; ++i) {
        pDescriptorSets[i] = (VkDescriptorSet)(uintptr_t)new VkDescriptorSet_T();
    }
    return 0;
}

void vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) {
    for (uint32_t i = 0; i < descriptorSetCount; ++i) {
        delete (VkDescriptorSet_T*)(uintptr_t)pDescriptorSets[i];
    }
}

void vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const void* pDescriptorWrites, uint32_t descriptorCopyCount, const void* pDescriptorCopies) {
    auto* writes = (const VkWriteDescriptorSet*)pDescriptorWrites;
    for (uint32_t i = 0; i < descriptorWriteCount; ++i) {
        const auto& write = writes[i];
        VkDescriptorSet_T* set = (VkDescriptorSet_T*)(uintptr_t)write.dstSet;
        GLenum glType = (write.descriptorType == 6) ? GL_UNIFORM_BUFFER : GL_SAMPLER_2D;
        GLuint resID = (glType == GL_UNIFORM_BUFFER) ? (GLuint)write.pBufferInfo->buffer : (GLuint)write.pImageInfo->imageView;
        set->resources[write.dstBinding] = {glType, resID};
    }
}

} 
