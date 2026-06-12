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

VkResult vkCreateShaderModule(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkShaderModule* pShaderModule) {
    const uint32_t* spirvCode = (const uint32_t*)((char*)pCreateInfo + 16);
    size_t spirvSize = *(size_t*)((char*)pCreateInfo + 12) / 4;

    spvc_context context = nullptr;
    spvc_context_create(&context);

    spvc_parsed_ir ir = nullptr;
    spvc_context_parse_spirv(context, spirvCode, spirvSize, &ir);

    spvc_compiler compiler = nullptr;
    spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler);

    spvc_compiler_options options = nullptr;
    spvc_compiler_create_compiler_options(compiler, &options);
    spvc_compiler_options_set_uint(options, SPVC_COMPILER_OPTION_GLSL_VERSION, 310);
    spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_ES, SPVC_TRUE);
    spvc_compiler_options_set_bool(options, SPVC_COMPILER_OPTION_GLSL_VULKAN_SEMANTICS, SPVC_FALSE);
    spvc_compiler_install_compiler_options(compiler, options);

    const char* glslRaw = nullptr;
    spvc_compiler_compile(compiler, &glslRaw);

    ShaderModule* module = (ShaderModule*)malloc(sizeof(ShaderModule));
    module->glslSource = strdup(glslRaw);
    module->shaderID = 0;

    spvc_context_destroy(context);

    *pShaderModule = (VkShaderModule)module;
    return VK_SUCCESS;
}

void vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const void* pAllocator) {
    if (shaderModule) {
        ShaderModule* module = (ShaderModule*)shaderModule;
        if (module->glslSource) free(module->glslSource);
        if (module->shaderID) glDeleteShader(module->shaderID);
        free(module);
    }
}

}
