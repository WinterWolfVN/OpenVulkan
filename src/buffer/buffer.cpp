#include "../fpa/struct_core.h"
#include <GLES3/gl3.h>
#include <cstring>
#include <cstdlib>

int32_t vkCreateBuffer(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkBuffer* pBuffer) {
    if (!device || !pBuffer) return -3;
    
    const auto* createInfo = static_cast<const VkBufferCreateInfo*>(pCreateInfo);
    
    VkBuffer buf = new VkBuffer_T();
    buf->size = createInfo->size;
    buf->usage = createInfo->usage;
    buf->memory = nullptr;
    buf->memoryOffset = 0;
    
    glGenBuffers(1, &buf->buffer);
    
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
            glDeleteBuffers(1, &buffer->buffer);
        }
        delete buffer;
    }
}

int32_t vkAllocateMemory(VkDevice device, const void* pAllocateInfo, const void* pAllocator, VkDeviceMemory* pMemory) {
    if (!device || !pAllocateInfo || !pMemory) return -3;
    
    const auto* allocInfo = static_cast<const VkMemoryAllocateInfo*>(pAllocateInfo);
    
    VkDeviceMemory mem = new VkDeviceMemory_T();
    mem->size = allocInfo->allocationSize;
    mem->memoryTypeIndex = allocInfo->memoryTypeIndex;
    mem->mappedCount = 0;
    mem->isMapped = false;
    mem->hostPtr = std::malloc(static_cast<size_t>(allocInfo->allocationSize));
    
    if (!mem->hostPtr && allocInfo->allocationSize > 0) {
        delete mem;
        return -3;
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

int32_t vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    if (!device || !buffer || !memory) return -3;
    
    buffer->memory = memory;
    buffer->memoryOffset = memoryOffset;
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(buffer->size), 
                 static_cast<char*>(memory->hostPtr) + memoryOffset, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return 0;
}

int32_t vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, uint32_t flags, void** ppData) {
    if (!device || !memory || !ppData) return -3;
    
    if (offset + size > memory->size) return -3;
    
    *ppData = static_cast<char*>(memory->hostPtr) + offset;
    memory->mappedCount++;
    memory->isMapped = true;
    
    return 0;
}

void vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    if (memory && memory->isMapped) {
        memory->mappedCount--;
        if (memory->mappedCount == 0) {
            memory->isMapped = false;
        }
    }
}
