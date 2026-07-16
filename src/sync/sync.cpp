#include "../struct/stsync.h"
#include <GLES3/gl31.h>
#include <new>
#include <cstring>

extern "C" {

void vkFlushFenceResetBag() {
    for (int32_t i = 0; i < g_fenceResetBagCount; ++i) {
        if (g_fenceResetBag[i]) glDeleteSync((GLsync)g_fenceResetBag[i]);
    }
    g_fenceResetBagCount = 0;
}

int32_t vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const void* pAllocator, VkFence* pFence) {
    if (!device || !pCreateInfo || !pFence) return -3;
    auto* fence = new(std::nothrow) VkFence_T();
    if (!fence) return -3;
    std::memset(fence, 0, sizeof(*fence));
    fence->fenceId = (int64_t)fence;
    fence->flags = pCreateInfo->flags;
    fence->signaled = (pCreateInfo->flags & 0x00000001) ? 1 : 0;
    fence->sync = nullptr;
    *pFence = fence;
    return 0;
}

void vkDestroyFence(VkDevice device, VkFence fence, const void* pAllocator) {
    if (!fence) return;
    if (fence->sync) {
        glDeleteSync((GLsync)fence->sync);
        fence->sync = nullptr;
    }
    delete fence;
}

int32_t vkResetFences(VkDevice device, int32_t fenceCount, const VkFence* pFences) {
    if (!device || !pFences) return -3;
    for (int32_t i = 0; i < fenceCount; ++i) {
        if (!pFences[i]) continue;
        VkFence_T* f = pFences[i];
        if (f->sync) {
            if (g_fenceResetBagCount < MAX_FENCE_RESET_BAG) {
                g_fenceResetBag[g_fenceResetBagCount++] = f->sync;
            } else {
                glDeleteSync((GLsync)f->sync);
            }
            f->sync = nullptr;
        }
        f->signaled = 0;
    }
    return 0;
}

int32_t vkWaitForFences(VkDevice device, int32_t fenceCount, const VkFence* pFences, int32_t waitAll, uint64_t timeout) {
    if (!device || !pFences) return -3;
    if (fenceCount == 0) return 0;    
    int32_t signaledCount = 0;
    for (int32_t i = 0; i < fenceCount; ++i) {
        if (!pFences[i]) continue;
        VkFence_T* f = pFences[i];
        if (f->signaled) {
            signaledCount++;
            continue;
        }
        if (f->sync) {
            GLenum res = glClientWaitSync((GLsync)f->sync, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
            if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED) {
                f->signaled = 1;
                signaledCount++;
            } else if (res == GL_TIMEOUT_EXPIRED) {
                if (waitAll) return -2;
            } else {
                return -3;
            }
        }
    }    
    if (waitAll && signaledCount == fenceCount) return 0;
    if (!waitAll && signaledCount > 0) return 0;
    return -2;
}

int32_t vkGetFenceStatus(VkDevice device, VkFence fence) {
    if (!device || !fence) return -3;
    if (fence->signaled) return 0;
    if (fence->sync) {
        GLint status = 0;
        glGetSynciv((GLsync)fence->sync, GL_SYNC_STATUS, 1, nullptr, &status);
        if (status == GL_SIGNALED) {
            fence->signaled = 1;
            return 0;
        }
    }
    return -1;
}

}
