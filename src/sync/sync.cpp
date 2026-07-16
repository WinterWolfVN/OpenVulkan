#include "../struct/stsync.h"
#include <GLES3/gl3.h>
#include <new>
#include <cstring>

extern "C" {

int32_t vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const void* pAllocator, VkFence* pFence) {
    if (!device || !pCreateInfo || !pFence) return -3;
    VkFence_T* fence = new(std::nothrow) VkFence_T();
    if (!fence) return -3;
    std::memset(fence, 0, sizeof(VkFence_T));
    fence->fenceId = (int64_t)fence;
    fence->flags = pCreateInfo->flags;
    fence->signaled = (pCreateInfo->flags & 1) ? 1 : 0;
    fence->sync = 0;
    *pFence = fence;
    return 0;
}

void vkDestroyFence(VkDevice device, VkFence fence, const void* pAllocator) {
    if (!fence) return;
    if (fence->sync) glDeleteSync(fence->sync);
    delete fence;
}

int32_t vkResetFences(VkDevice device, int32_t fenceCount, const VkFence* pFences) {
    if (!device || !pFences) return -3;    
    for (int32_t i = 0; i < g_ResetBagCount; ++i) {
        if (g_ResetBag[i]) glDeleteSync(g_ResetBag[i]);
    }
    g_ResetBagCount = 0;    
    for (int32_t i = 0; i < fenceCount; ++i) {
        VkFence_T* fence = pFences[i];
        if (!fence) continue;        
        if (fence->sync) {
            if (g_ResetBagCount < MAX_RESET_BAG) {
                g_ResetBag[g_ResetBagCount++] = fence->sync;
            } else {
                glDeleteSync(fence->sync);
            }
            fence->sync = 0;
        }
        fence->signaled = 0;
    }
    return 0;
}

int32_t vkWaitForFences(VkDevice device, int32_t fenceCount, const VkFence* pFences, int32_t waitAll, uint64_t timeout) {
    if (!device || !pFences) return -3;
    int32_t signaledCount = 0;
    for (int32_t i = 0; i < fenceCount; ++i) {
        VkFence_T* fence = pFences[i];
        if (!fence) continue;
        if (fence->signaled) { signaledCount++; continue; }
        if (!fence->sync) continue;
        GLenum result = glClientWaitSync(fence->sync, 0, timeout);
        if (result == GL_WAIT_FAILED) return -3;
        if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
            fence->signaled = 1;
            signaledCount++;
        }
        if (!waitAll && signaledCount > 0) return 0;
    }
    if (waitAll && signaledCount == fenceCount) return 0;
    return 1;
}

int32_t vkGetFenceStatus(VkDevice device, VkFence fence) {
    if (!device || !fence) return -3;
    if (fence->signaled) return 0;
    if (!fence->sync) return 1;
    GLenum result = glClientWaitSync(fence->sync, 0, 0);
    if (result == GL_WAIT_FAILED) return -3;
    if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
        fence->signaled = 1;
        return 0;
    }
    return 1;
}

}
