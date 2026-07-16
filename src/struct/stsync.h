#pragma once
#include <cstdint>

#define MAX_RESET_BAG 256

static GLsync g_ResetBag[MAX_RESET_BAG];
static int32_t g_ResetBagCount = 0;

struct VkFenceCreateInfo {
    const void* pNext;    
    int32_t sType;
    int32_t flags;
};

struct VkFence_T {
    void* sync;
    int64_t fenceId;    
    int32_t signaled;
    int32_t flags;
};
typedef struct VkFence_T* VkFence;
