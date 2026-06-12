#include <EGL/egl.h>
#include <GLES3/gl31.h>
#include <string.h>

typedef void* VkInstance;
typedef void* VkPhysicalDevice;
typedef void* VkDevice;
typedef uint32_t VkResult;
typedef void (*PFN_vkVoidFunction)(void);
typedef uint64_t VkDeviceSize;
typedef uint64_t VkBuffer;
typedef uint64_t VkDeviceMemory;

#define VK_SUCCESS 0
#define VK_ERROR_INITIALIZATION_FAILED -3

EGLDisplay global_display = EGL_NO_DISPLAY;
EGLContext global_context = EGL_NO_CONTEXT;
EGLConfig  global_config;
GLuint global_fake_vbo = 0;

extern "C" {

VkResult vkCreateInstance(const void* pCreateInfo, const void* pAllocator, VkInstance* pInstance) {
    global_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (global_display == EGL_NO_DISPLAY) return VK_ERROR_INITIALIZATION_FAILED;
    if (!eglInitialize(global_display, nullptr, nullptr)) return VK_ERROR_INITIALIZATION_FAILED;
    *pInstance = (VkInstance)&global_display;
    return VK_SUCCESS;
}

VkResult vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
    if (pPhysicalDevices) *pPhysicalDevices = (VkPhysicalDevice)1;
    *pPhysicalDeviceCount = 1;
    return VK_SUCCESS;
}

void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, void* pProperties) {
    if (pProperties) {
        memset(pProperties, 0, 1024);
        strcpy((char*)pProperties + 16, "GLES 3.1 Bridge Android 5");
    }
}

void vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, void* pMemoryProperties) {
    if (pMemoryProperties) {
        memset(pMemoryProperties, 0, 512);
        uint32_t* ptr = (uint32_t*)pMemoryProperties;
        ptr[0] = 1; 
        ptr[1] = 0x0000000F; 
    }
}

VkResult vkCreateDevice(VkPhysicalDevice physicalDevice, const void* pCreateInfo, const void* pAllocator, VkDevice* pDevice) {
    const EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24, EGL_NONE
    };
    EGLint numConfigs;
    eglChooseConfig(global_display, configAttribs, &global_config, 1, &numConfigs);
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    global_context = eglCreateContext(global_display, global_config, EGL_NO_CONTEXT, contextAttribs);
    *pDevice = (VkDevice)&global_context;
    return VK_SUCCESS;
}

VkResult vkCreateBuffer(VkDevice device, const void* pCreateInfo, const void* pAllocator, VkBuffer* pBuffer) {
    if (!global_fake_vbo) {
        glGenBuffers(1, &global_fake_vbo);
    }
    *pBuffer = (VkBuffer)global_fake_vbo;
    return VK_SUCCESS;
}

void vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, void* pMemoryRequirements) {
    if (pMemoryRequirements) {
        VkDeviceSize* sizePtr = (VkDeviceSize*)pMemoryRequirements;
        sizePtr[0] = 65536; 
        sizePtr[1] = 16;    
        uint32_t* typePtr = (uint32_t*)((char*)pMemoryRequirements + 16);
        *typePtr = 1;
    }
}

VkResult vkAllocateMemory(VkDevice device, const void* pAllocateInfo, const void* pAllocator, VkDeviceMemory* pMemory) {
    *pMemory = (VkDeviceMemory)1;
    return VK_SUCCESS;
}

VkResult vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint)buffer);
    return VK_SUCCESS;
}

VkResult vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, uint32_t flags, void** ppData) {
    static char fake_ram_buffer[65536];
    *ppData = fake_ram_buffer;
    return VK_SUCCESS;
}

void vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
} 

void vkCmdBindVertexBuffers(void* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) {
    if (bindingCount > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, (GLuint)pBuffers[0]);
    }
}

extern VkResult vkQueuePresentKHR(void* queue, const void* pPresentInfo);
extern void vkCmdBeginRenderPass(void* commandBuffer, const void* pBegin, uint32_t contents);
extern void vkCmdEndRenderPass(void* commandBuffer);
extern VkResult vkAcquireNextImageKHR(void* device, void* swapchain, uint64_t timeout, void* sem, void* fence, uint32_t* pIndex);
extern void vkCmdDraw(void* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    if (strcmp(pName, "vkCreateInstance") == 0) return (PFN_vkVoidFunction)vkCreateInstance;
    if (strcmp(pName, "vkCreateDevice") == 0) return (PFN_vkVoidFunction)vkCreateDevice;
    if (strcmp(pName, "vkEnumeratePhysicalDevices") == 0) return (PFN_vkVoidFunction)vkEnumeratePhysicalDevices;
    if (strcmp(pName, "vkGetPhysicalDeviceProperties") == 0) return (PFN_vkVoidFunction)vkGetPhysicalDeviceProperties;
    if (strcmp(pName, "vkGetPhysicalDeviceMemoryProperties") == 0) return (PFN_vkVoidFunction)vkGetPhysicalDeviceMemoryProperties;
    if (strcmp(pName, "vkCreateBuffer") == 0) return (PFN_vkVoidFunction)vkCreateBuffer;
    if (strcmp(pName, "vkGetBufferMemoryRequirements") == 0) return (PFN_vkVoidFunction)vkGetBufferMemoryRequirements;
    if (strcmp(pName, "vkAllocateMemory") == 0) return (PFN_vkVoidFunction)vkAllocateMemory;
    if (strcmp(pName, "vkBindBufferMemory") == 0) return (PFN_vkVoidFunction)vkBindBufferMemory;
    if (strcmp(pName, "vkMapMemory") == 0) return (PFN_vkVoidFunction)vkMapMemory;
    if (strcmp(pName, "vkUnmapMemory") == 0) return (PFN_vkVoidFunction)vkUnmapMemory;
    if (strcmp(pName, "vkCmdBindVertexBuffers") == 0) return (PFN_vkVoidFunction)vkCmdBindVertexBuffers;
    if (strcmp(pName, "vkCmdBeginRenderPass") == 0) return (PFN_vkVoidFunction)vkCmdBeginRenderPass;
    if (strcmp(pName, "vkCmdEndRenderPass") == 0) return (PFN_vkVoidFunction)vkCmdEndRenderPass;
    if (strcmp(pName, "vkQueuePresentKHR") == 0) return (PFN_vkVoidFunction)vkQueuePresentKHR;
    if (strcmp(pName, "vkAcquireNextImageKHR") == 0) return (PFN_vkVoidFunction)vkAcquireNextImageKHR;
    if (strcmp(pName, "vkCmdDraw") == 0) return (PFN_vkVoidFunction)vkCmdDraw;
    return nullptr;
}

PFN_vkVoidFunction vkGetDeviceProcAddr(VkDevice device, const char* pName) {
    return vkGetInstanceProcAddr(nullptr, pName);
}

}
