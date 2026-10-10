#pragma once
#include <cstdint>

struct VkShaderModule_T {
    char* code;
    int64_t codeSize;
};
typedef struct VkShaderModule_T* VkShaderModule;

struct VkShaderModuleCreateInfo {
    const void* pNext;
    const uint32_t* pCode;    
    int64_t codeSize;    
    int32_t sType;
    uint32_t flags;
};