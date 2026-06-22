#include "../struct/stcore.h"
#include <GLES3/gl31.h>
#include <cstring>
#include <cstdlib>

#ifndef VK_WHOLE_SIZE
#define VK_WHOLE_SIZE (-1LL)
#endif

extern "C" {

int32_t vkCreateBuffer(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkBuffer* pBuffer) {
    if (!device || !pCreateInfo || !pBuffer) return -3;
    
    const VkBufferCreateInfo* createInfo = static_cast<const VkBufferCreateInfo*>(pCreateInfo);
    
    if (createInfo->size < 0) return -3;
    
    VkBuffer buf = new VkBuffer_T();
    buf->buffer = 0;
    buf->size = static_cast<int64_t>(createInfo->size);
    buf->usage = static_cast<int32_t>(createInfo->usage);
    buf->memory = nullptr;
    buf->memoryOffset = 0;
    
    GLuint gl_buf = 0;
    glGenBuffers(1, &gl_buf);
    buf->buffer = static_cast<int32_t>(gl_buf);
    
    if (buf->buffer == 0) {
        delete buf;
        return -3;
    }
    
    *pBuffer = buf;
    return 0;
}

void vkDestroyBuffer(VkDevice device, VkBuffer buffer, const void* pAllocator) {
    if (buffer) {
        if (buffer->buffer != 0) {
            GLuint gl_buf = static_cast<GLuint>(buffer->buffer);
            glDeleteBuffers(1, &gl_buf);
        }
        delete buffer;
    }
}

int32_t vkAllocateMemory(VkDevice device, const void* pAllocateInfo, const void* pAllocator, VkDeviceMemory* pMemory) {
    if (!device || !pAllocateInfo || !pMemory) return -3;
    
    const VkMemoryAllocateInfo* allocInfo = static_cast<const VkMemoryAllocateInfo*>(pAllocateInfo);
    
    if (allocInfo->allocationSize < 0) return -3;
    
    VkDeviceMemory mem = new VkDeviceMemory_T();
    mem->size = static_cast<int64_t>(allocInfo->allocationSize);
    mem->memoryTypeIndex = static_cast<int32_t>(allocInfo->memoryTypeIndex);
    mem->mappedCount = 0;
    mem->isMapped = 0;
    
    if (allocInfo->allocationSize == 0) {
        mem->hostPtr = nullptr;
    } else {
        mem->hostPtr = std::malloc(static_cast<size_t>(allocInfo->allocationSize));
        if (!mem->hostPtr) {
            delete mem;
            return -3;
        }
    }
    
    *pMemory = mem;
    return 0;
}

void vkFreeMemory(VkDevice device, VkDeviceMemory memory, const void* pAllocator) {
    if (memory) {
        if (memory->hostPtr) {
            std::free(memory->hostPtr);
        }
        delete memory;
    }
}

int32_t vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, int64_t memoryOffset) {
    if (!device || !buffer || !memory) return -3;
    
    if (memoryOffset < 0 || buffer->size < 0) return -3;
    if (memoryOffset > memory->size) return -3;
    if (buffer->size > memory->size - memoryOffset) return -3;
    
    buffer->memory = memory;
    buffer->memoryOffset = memoryOffset;
    
    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(buffer->buffer));
    
    void* dataPtr = nullptr;
    if (memory->hostPtr && buffer->size > 0) {
        dataPtr = static_cast<char*>(memory->hostPtr) + static_cast<size_t>(memoryOffset);
    }
    
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(buffer->size), dataPtr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return 0;
}

int32_t vkMapMemory(VkDevice device, VkDeviceMemory memory, int64_t offset, int64_t size, int32_t flags, void** ppData) {
    if (!device || !memory || !ppData) return -3;
    
    if (offset < 0) return -3;
    if (offset > memory->size) return -3;
    
    int64_t mapSize = size;
    if (size == VK_WHOLE_SIZE) {
        mapSize = memory->size - offset;
    } else {
        if (size < 0) return -3;
        if (size > memory->size - offset) return -3;
    }
    
    if (mapSize == 0) {
        *ppData = nullptr;
    } else {
        *ppData = static_cast<char*>(memory->hostPtr) + static_cast<size_t>(offset);
    }
    
    memory->mappedCount++;
    memory->isMapped = 1;
    
    (void)flags;
    return 0;
}

void vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    if (!memory || !memory->isMapped) return;
    
    if (memory->mappedCount.load() > 0) {
        memory->mappedCount--;
        if (memory->mappedCount.load() == 0) {
            memory->isMapped = 0;
        }
    }
}

int32_t vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const void* pAllocator, VkCommandPool* pCommandPool) {
    if (!device || !pCreateInfo || !pCommandPool) return -3;
    
    VkCommandPool pool = new VkCommandPool_T();
    pool->flags = pCreateInfo->flags;
    pool->queueFamilyIndex = pCreateInfo->queueFamilyIndex;
    
    *pCommandPool = pool;
    return 0;
}

void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const void* pAllocator) {
    if (commandPool) {
        delete commandPool;
    }
}

int32_t vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {
    if (!device || !pAllocateInfo || !pCommandBuffers) return -3;
    
    for (int32_t i = 0; i < pAllocateInfo->commandBufferCount; ++i) {
        VkCommandBuffer cmdBuf = new VkCommandBuffer_T();
        cmdBuf->currentTopology = 0;
        cmdBuf->currentIndexType = 0;
        cmdBuf->currentIndexOffset = 0;
        
        pCommandBuffers[i] = cmdBuf;
    }
    
    return 0;
}

void vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, int32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    if (!pCommandBuffers) return;
    
    for (int32_t i = 0; i < commandBufferCount; ++i) {
        if (pCommandBuffers[i]) {
            delete pCommandBuffers[i];
        }
    }
}

int32_t vkResetCommandBuffer(VkCommandBuffer commandBuffer, int32_t flags) {
    if (!commandBuffer) return -3;
    
    commandBuffer->currentTopology = 0;
    commandBuffer->currentIndexType = 0;
    commandBuffer->currentIndexOffset = 0;
    
    return 0;
}

int32_t vkResetCommandPool(VkDevice device, VkCommandPool commandPool, int32_t flags) {
    if (!device || !commandPool) return -3;
    
    return 0;
}

} 
