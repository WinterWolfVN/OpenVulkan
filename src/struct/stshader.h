#pragma once
#include <cstdint>

struct VkShaderModule_T {
    char* code;
    int64_t codeSize;
};
typedef struct VkShaderModule_T* VkShaderModule;

struct VkShaderModuleCreateInfo {
    int32_t sType;
    const void* pNext;
    int32_t flags;
    int64_t codeSize;
    const uint32_t* pCode;
};
typedef struct VkShaderModuleCreateInfo VkShaderModuleCreateInfo;
