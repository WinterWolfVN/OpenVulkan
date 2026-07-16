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
    int64_t fenceId;
    GLsync sync;    
    int32_t signaled;
    int32_t flags;
};
typedef struct VkFence_T* VkFence;

struct VkEventCreateInfo {
    const void* pNext;    
    int32_t sType;
    int32_t flags;
};

struct VkEvent_T {
    int64_t eventId;
    GLsync sync;    
    int32_t signaled;
    int32_t flags;
};
typedef struct VkEvent_T* VkEvent;
