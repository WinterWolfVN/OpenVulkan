#include "../struct/stsync.h"
#include <GLES3/gl3.h>
#include <new>
#include <cstring>

extern "C" {

int32_t vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const void* pAllocator, VkSemaphore* pSemaphore) {
    if (!device || !pCreateInfo || !pSemaphore) return -3;
    VkSemaphore_T* sem = new(std::nothrow) VkSemaphore_T();
    if (!sem) return -3;
    std::memset(sem, 0, sizeof(VkSemaphore_T));
    sem->semaphoreId = (int64_t)sem;
    sem->type = 0;
    sem->currentValue = 0;
    sem->pendingValue = 0;
    sem->pendingSync = 0;
    sem->signaled = 0;
    sem->flags = pCreateInfo->flags;    
    const void* pNext = pCreateInfo->pNext;
    while (pNext) {
        int32_t sType = *(const int32_t*)pNext;
        if (sType == 1000207001) {
            const VkSemaphoreTypeCreateInfo* typeInfo = (const VkSemaphoreTypeCreateInfo*)pNext;
            sem->type = typeInfo->semaphoreType;
            sem->currentValue = typeInfo->initialValue;
            break;
        }
        pNext = *(const void**)pNext;
    }    
    *pSemaphore = sem;
    return 0;
}

void vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const void* pAllocator) {
    if (!semaphore) return;
    if (semaphore->pendingSync) glDeleteSync(semaphore->pendingSync);
    delete semaphore;
}

int32_t vkGetSemaphoreCounterValue(VkDevice device, VkSemaphore semaphore, uint64_t* pValue) {
    if (!device || !semaphore || !pValue) return -3;
    if (semaphore->type != 2) return -3;
    
    if (semaphore->pendingSync) {
        GLenum res = glClientWaitSync(semaphore->pendingSync, 0, 0);
        if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED) {
            semaphore->currentValue = semaphore->pendingValue;
            glDeleteSync(semaphore->pendingSync);
            semaphore->pendingSync = 0;
        }
    }
    *pValue = semaphore->currentValue;
    return 0;
}

int32_t vkSignalSemaphore(VkDevice device, const VkSemaphoreSignalInfo* pSignalInfo) {
    if (!device || !pSignalInfo) return -3;
    VkSemaphore_T* sem = pSignalInfo->semaphore;
    if (!sem || sem->type != 2) return -3;
    sem->currentValue = pSignalInfo->value;
    return 0;
}

int32_t vkWaitSemaphores(VkDevice device, const VkSemaphoreWaitInfo* pWaitInfo, uint64_t timeout) {
    if (!device || !pWaitInfo) return -3;    
    for (int32_t i = 0; i < pWaitInfo->semaphoreCount; ++i) {
        VkSemaphore_T* sem = pWaitInfo->pSemaphores[i];
        uint64_t waitVal = pWaitInfo->pValues[i];
        if (!sem) continue;        
        if (sem->type == 2) {
            while (sem->currentValue < waitVal) {
                if (!sem->pendingSync) return -3;
                GLenum res = glClientWaitSync(sem->pendingSync, 0, timeout);
                if (res == GL_WAIT_FAILED) return -3;
                if (res == GL_TIMEOUT_EXPIRED) return 1;
                if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED) {
                    sem->currentValue = sem->pendingValue;
                    glDeleteSync(sem->pendingSync);
                    sem->pendingSync = 0;
                }
            }
        } else {
            if (!sem->signaled) {
                if (sem->pendingSync) {
                    GLenum res = glClientWaitSync(sem->pendingSync, 0, timeout);
                    if (res == GL_WAIT_FAILED) return -3;
                    if (res == GL_TIMEOUT_EXPIRED) return 1;
                    if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED) {
                        sem->signaled = 1;
                        glDeleteSync(sem->pendingSync);
                        sem->pendingSync = 0;
                    }
                } else {
                    return -3;
                }
            }
        }
    }
    return 0;
}

}
