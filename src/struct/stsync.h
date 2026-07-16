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

struct VkSemaphoreTypeCreateInfo {
    const void* pNext;
    int64_t initialValue;    
    int32_t sType;
    int32_t semaphoreType;
};

struct VkSemaphoreCreateInfo {
    const void* pNext;    
    int32_t sType;
    int32_t flags;
};

struct VkSemaphoreSignalInfo {
    const void* pNext;
    VkSemaphore semaphore;
    uint64_t value;    
    int32_t sType;
};

struct VkSemaphoreWaitInfo {
    const void* pNext;
    const VkSemaphore* pSemaphores;
    const uint64_t* pValues;    
    int32_t sType;
    int32_t flags;
    int32_t semaphoreCount;
};

struct VkSemaphore_T {
    int64_t semaphoreId;
    uint64_t currentValue;
    uint64_t pendingValue;
    GLsync pendingSync;   
    int32_t type;
    int32_t signaled;
    int32_t flags;
};
typedef struct VkSemaphore_T* VkSemaphore;
