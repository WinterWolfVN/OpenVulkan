#include <GLES3/gl31.h>
#include <spirv_cross/spirv_cross_c.h>
#include <vector>
#include <map>

typedef void* VkDevice;
typedef uint32_t VkResult;
typedef uint64_t VkShaderModule;
typedef uint64_t VkDescriptorSet;

struct DescriptorResource { GLenum type; GLuint id; };
struct ShaderModuleContext { GLuint shaderID; std::vector<uint32_t> bindings; };
struct DescriptorSetState { std::map<uint32_t, DescriptorResource> resources; };

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
        ctx->bindings.push_back(glsl.get_decoration(r.id, spv::DecorationBinding));
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
VkDescriptorSet vkCreateDescriptorSet(VkDevice device) {
    return (VkDescriptorSet)(uintptr_t)new DescriptorSetState();
}

void vkUpdateDescriptorSets(VkDescriptorSet set, uint32_t binding, GLuint bufferID) {
    auto* state = (DescriptorSetState*)(uintptr_t)set;
    state->resources[binding] = {GL_UNIFORM_BUFFER, bufferID};
}

void vkCmdBindDescriptorSets(GLuint program, VkDescriptorSet set) {
    auto* state = (DescriptorSetState*)(uintptr_t)set;
    for (auto const& [binding, res] : state->resources) {
        GLuint blockIndex = glGetUniformBlockIndex(program, "GlobalData");
        glUniformBlockBinding(program, blockIndex, binding);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, res.id);
    }
}

}
