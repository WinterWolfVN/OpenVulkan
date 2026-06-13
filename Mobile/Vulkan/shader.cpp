#include <GLES3/gl31.h>
#include <spirv_cross/spirv_cross_c.h>
#include <vector>
#include <map>
#include <string.h>

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
    auto* info = (const VkShaderModuleCreateInfo*)pCreateInfo;
    spirv_cross::CompilerGLSL glsl((uint32_t*)info->pCode, info->codeSize / sizeof(uint32_t));
    
    std::string source = glsl.compile();
    const char* srcPtr = source.c_str();
    GLuint shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader, 1, &srcPtr, NULL);
    glCompileShader(shader);

    ShaderModuleContext* ctx = new ShaderModuleContext();
    ctx->shaderID = shader;

    auto res = glsl.get_shader_resources();
    for (auto &r : res.uniform_buffers) {
        ctx->descriptors.push_back({glsl.get_decoration(r.id, spv::DecorationDescriptorSet), glsl.get_decoration(r.id, spv::DecorationBinding), GL_UNIFORM_BUFFER});
    }
    for (auto &r : res.sampled_images) {
        ctx->descriptors.push_back({glsl.get_decoration(r.id, spv::DecorationDescriptorSet), glsl.get_decoration(r.id, spv::DecorationBinding), GL_SAMPLER_2D});
    }
    
    *pShaderModule = (VkShaderModule)(uintptr_t)ctx;
    return 0;
}

void vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const void* pAllocator) {
    ShaderModuleContext* ctx = (ShaderModuleContext*)(uintptr_t)shaderModule;
    glDeleteShader(ctx->shaderID);
    delete ctx;
}

// --- Descriptor Sets --- 
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

void vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, uint32_t pipelineBindPoint, uint64_t layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) {
    auto& cmd = *(CommandBuffer*)commandBuffer;    
    Command c;
    c.type = CMD_BIND_DESCRIPTOR_SETS;
    c.descriptorSet = pDescriptorSets[0];
    cmd.commands.push_back(c);
}

} 
