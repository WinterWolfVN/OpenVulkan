#include "../struct/stshader.h"
#include <GLES3/gl31.h>
#include <cstring>
#include <cstdlib>

extern "C" {

int32_t vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const void* pAllocator, VkShaderModule* pShaderModule) {
    if (!device || !pCreateInfo || !pShaderModule) return -3;
    VkShaderModule module = new VkShaderModule_T();
    module->codeSize = pCreateInfo->codeSize;
    module->code = static_cast<char*>(std::malloc(static_cast<size_t>(pCreateInfo->codeSize)));
    if (!module->code) {
        delete module;
        return -3;
    }
    std::memcpy(module->code, pCreateInfo->pCode, static_cast<size_t>(pCreateInfo->codeSize));
    *pShaderModule = module;
    return 0;
}

void vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const void* pAllocator) {
    if (shaderModule) {
        if (shaderModule->code) {
            std::free(shaderModule->code);
        }
        delete shaderModule;
    }
}

    }
