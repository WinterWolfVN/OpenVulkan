#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "struct.h"
#include <cstring>
#include <cstdlib>

int32_t vkCreateInstance(const void* pCreateInfo, const void* pAllocator, VkInstance* pInstance) {
    VkInstance inst = new VkInstance_T();
    inst->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(inst->display, nullptr, nullptr);
    *pInstance = inst;
    return 0; 
}

int32_t vkEnumeratePhysicalDevices(VkInstance instance, int32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
    if (pPhysicalDevices == nullptr) {
        *pPhysicalDeviceCount = 1; 
        return 0;
    }
    VkPhysicalDevice gpu = new VkPhysicalDevice_T();
    gpu->gpu_name = (const char*)glGetString(GL_RENDERER);
    if (!gpu->gpu_name) {
        gpu->gpu_name = "Mali-G Series GPU";
    }
    pPhysicalDevices[0] = gpu;
    *pPhysicalDeviceCount = 1;
    return 0;
}

void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) {
    std::memset(pProperties, 0, sizeof(VkPhysicalDeviceProperties));
    pProperties->apiVersion = (1 << 22) | (4 << 12); 
    pProperties->driverVersion = 1;
    pProperties->vendorID = 0x13B5; 
    pProperties->deviceID = 0x7120; 
    std::strncpy(pProperties->deviceName, physicalDevice->gpu_name, 255);
}

void vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    pMemoryProperties->memoryHeapCount = 2;
    pMemoryProperties->memoryHeaps[0].size = 536870912LL; 
    pMemoryProperties->memoryHeaps[0].flags = 1;     
    pMemoryProperties->memoryHeaps[1].size = 1073741824LL; 
    pMemoryProperties->memoryHeaps[1].flags = 0;
    pMemoryProperties->memoryTypeCount = 2;
    pMemoryProperties->memoryTypes[0].propertyFlags = 0x00000001; 
    pMemoryProperties->memoryTypes[0].heapIndex = 0;
    pMemoryProperties->memoryTypes[1].propertyFlags = 0x00000006; 
    pMemoryProperties->memoryTypes[1].heapIndex = 1;
}

int32_t vkCreateDevice(VkPhysicalDevice physicalDevice, const void* pCreateInfo, const void* pAllocator, VkDevice* pDevice) {
    VkDevice dev = new VkDevice_T();
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);    
    EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8,
        EGL_NONE
    };    
    EGLConfig config;
    EGLint num_configs;
    eglChooseConfig(dpy, config_attribs, &config, 1, &num_configs);    
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };    
    dev->context = eglCreateContext(dpy, config, EGL_NO_CONTEXT, context_attribs);
    dev->surface = EGL_NO_SURFACE; 
    eglMakeCurrent(dpy, dev->surface, dev->surface, dev->context);    
    *pDevice = dev;
    return 0; 
}
