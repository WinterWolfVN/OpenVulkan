#include <GLES3/gl32.h>
#include <stdlib.h>

typedef void* VkInstance;
typedef void* VkPhysicalDevice;
typedef void* VkDevice;
typedef uint32_t VkResult;

struct VkInstance_T { GLuint dummy; };
struct VkDevice_T { GLuint dummy; };

extern "C" {

VkResult vkCreateInstance(const void* pCreateInfo, const void* pAllocator, VkInstance* pInstance) {
    VkInstance_T* instance = new VkInstance_T();
    *pInstance = (VkInstance)instance;
    return 0;
}

void vkDestroyInstance(VkInstance instance, const void* pAllocator) {
    delete (VkInstance_T*)instance;
}

VkResult vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
    if (pPhysicalDevices) {
        *pPhysicalDeviceCount = 1;
        pPhysicalDevices[0] = (VkPhysicalDevice)1;
    } else {
        *pPhysicalDeviceCount = 1;
    }
    return 0;
}

void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, void* pProperties) {} 
    
void vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, void* pMemoryProperties) {} 
    
VkResult vkCreateDevice(VkPhysicalDevice physicalDevice, const void* pCreateInfo, const void* pAllocator, VkDevice* pDevice) {
    VkDevice_T* device = new VkDevice_T();
    *pDevice = (VkDevice)device;
    return 0;
}

void vkDestroyDevice(VkDevice device, const void* pAllocator) {
    delete (VkDevice_T*)device;
}

VkResult vkCreateFramebuffer(
    VkDevice device,
    const VkFramebufferCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkFramebuffer* pFramebuffer) 
{
    VkFramebuffer fb = new VkFramebuffer_T();
    fb->width = pCreateInfo->width;
    fb->height = pCreateInfo->height;    
    glGenFramebuffers(1, &fb->gles_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->gles_fbo);    
    for (uint32_t i = 0; i < pCreateInfo->attachmentCount; ++i) {
        VkImageView attachment = pCreateInfo->pAttachments[i];
        if (!attachment) continue;        
        if (attachment->is_depth) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, attachment->gles_tex_id, 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachment->gles_tex_id, 0);
        }
    }    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {        
    }    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    *pFramebuffer = fb;
    return VK_SUCCESS;
}

void vkDestroyFramebuffer(
    VkDevice device,
    VkFramebuffer framebuffer,
    const VkAllocationCallbacks* pAllocator) 
{
    if (framebuffer) {
        if (framebuffer->gles_fbo != 0) {
            glDeleteFramebuffers(1, &framebuffer->gles_fbo);
        }
        delete framebuffer;
    }
}

}
