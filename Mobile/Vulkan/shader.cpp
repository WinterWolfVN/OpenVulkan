#include <GLES3/gl31.h>
#include <string.h>
#include <stdlib.h>
#include <spirv_cross/spirv_cross_c.h>

typedef void* VkDevice;
typedef uint32_t VkResult;
typedef uint64_t VkShaderModule;

#define VK_SUCCESS 0

struct ShaderModule {
    GLuint shaderID;
    char* glslSource;
};

extern "C" {

VkResult vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const void* pAllocator, VkShaderModule* pShaderModule) {
    if (!pCreateInfo || !pShaderModule) return VK_ERROR_INITIALIZATION_FAILED;

    spirv_cross::CompilerGLSL glsl(pCreateInfo->pCode, pCreateInfo->codeSize / sizeof(uint32_t));

    spirv_cross::CompilerGLSL::Options options;
    options.version = 310;
    options.es = true;
    glsl.set_common_options(options);

    std::string source = glsl.compile();
    const char* srcPtr = source.c_str();

    GLuint shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader, 1, &srcPtr, NULL);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        glDeleteShader(shader);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    *pShaderModule = (VkShaderModule)(uintptr_t)shader;
    return VK_SUCCESS;
}

void vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const void* pAllocator) {
    if (shaderModule != VK_NULL_HANDLE) {
        glDeleteShader((GLuint)(uintptr_t)shaderModule);
    }
}

}
