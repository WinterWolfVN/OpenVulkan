#include "stsync.h"
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

int32_t vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences) {
    if (!device || !pFences) return -3;
    for (int32_t i = 0; i < ResetCacheCount; ++i) {
        ResetCache[i] = 0;
    }
    ResetCacheCount = 0;
    for (uint32_t i = 0; i < fenceCount; ++i) {
        VkFence_T* fence = pFences[i];
        if (!fence) continue;
        if (fence->sync) {
            glDeleteSync(fence->sync);
            fence->sync = 0;
        }
        fence->signaled = 0;
        if (ResetCacheCount < MaxResetCache) {
            ResetCache[ResetCacheCount++] = fence;
        }
    }
    return 0;
}

int32_t vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, int32_t waitAll, uint64_t timeout) {
    if (!device || !pFences) return -3;
    int32_t signaledCount = 0;
    for (uint32_t i = 0; i < fenceCount; ++i) {
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

int32_t vkCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo, const void* pAllocator, VkEvent* pEvent) {
    if (!device || !pCreateInfo || !pEvent) return -3;
    VkEvent_T* ev = new(std::nothrow) VkEvent_T();
    if (!ev) return -3;
    std::memset(ev, 0, sizeof(VkEvent_T));
    ev->eventId = (int64_t)ev;
    ev->flags = pCreateInfo->flags;
    ev->signaled = 0;
    ev->sync = 0;
    *pEvent = ev;
    return 0;
}

void vkDestroyEvent(VkDevice device, VkEvent event, const void* pAllocator) {
    if (!event) return;
    if (event->sync) glDeleteSync(event->sync);
    delete event;
}

int32_t vkSetEvent(VkDevice device, VkEvent event) {
    if (!device || !event) return -3;
    event->signaled = 1;
    return 0;
}

int32_t vkResetEvent(VkDevice device, VkEvent event) {
    if (!device || !event) return -3;
    event->signaled = 0;
    if (event->sync) {
        glDeleteSync(event->sync);
        event->sync = 0;
    }
    return 0;
}

int32_t vkGetEventStatus(VkDevice device, VkEvent event) {
    if (!device || !event) return -3;
    if (event->signaled) return 0;
    if (event->sync) {
        GLenum res = glClientWaitSync(event->sync, 0, 0);
        if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED) {
            event->signaled = 1;
            return 0;
        }
    }
    return 1;
}

int32_t vkQueueWaitIdle(VkQueue queue) {
    if (!queue) return -3;
    glFinish();
    return 0;
}

int32_t vkDeviceWaitIdle(VkDevice device) {
    if (!device) return -3;
    glFinish();
    return 0;
}

}
