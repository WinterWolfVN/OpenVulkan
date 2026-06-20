#include "../fpa/struct_context.h"
#include <GLES3/gl3.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

static VkPhysicalDevice_T g_cached_gpu;
static std::atomic<int32_t> g_gpu_initialized{0};
static std::mutex g_gpu_init_mutex;

int32_t vkCreateInstance(const void* pCreateInfo, const void* pAllocator, VkInstance* pInstance) {
    if (!pInstance) return -3;
    
    VkInstance inst = new VkInstance_T();
    inst->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    
    if (inst->display == EGL_NO_DISPLAY) {
        delete inst;
        return -3;
    }
    EGLint major = 0, minor = 0;
    if (!eglInitialize(inst->display, nullptr, nullptr)) {
        delete inst;
        return -3;
        (void)major; (void)minor;
    }
    
    *pInstance = inst;
    return 0; 
}

void vkDestroyInstance(VkInstance instance, const void* pAllocator) {
    if (instance) {
        if (instance->display != EGL_NO_DISPLAY) {
            eglTerminate(instance->display);
        }
        delete instance;
    }
}

int32_t vkEnumeratePhysicalDevices(VkInstance instance, int32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
    if (!instance || !pPhysicalDeviceCount) return -3;

    if (pPhysicalDevices == nullptr) {
        *pPhysicalDeviceCount = 1; 
        return 0;
    }

    std::lock_guard<std::mutex> lock(g_gpu_init_mutex);

    if (!g_gpu_initialized) {
        EGLint attribs[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };
        EGLConfig config;
        EGLint num_configs;
        if (!eglChooseConfig(instance->display, attribs, &config, 1, &num_configs) || num_configs == 0) {
            return -3;
        }

        EGLint ctx_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
        EGLContext temp_ctx = eglCreateContext(instance->display, config, EGL_NO_CONTEXT, ctx_attribs);        
        if (temp_ctx == EGL_NO_CONTEXT) return -3;

        EGLint pbuf_attribs[] = { EGL_WIDTH, 16, EGL_HEIGHT, 16, EGL_NONE };
        EGLSurface temp_surf = eglCreatePbufferSurface(instance->display, config, pbuf_attribs);        
        if (temp_surf == EGL_NO_SURFACE) {
            eglDestroyContext(instance->display, temp_ctx);
            return -3;
        }

        if (eglMakeCurrent(instance->display, temp_surf, temp_surf, temp_ctx)) {
            const char* renderer = (const char*)glGetString(GL_RENDERER);
            if (renderer) {
                std::snprintf(g_cached_gpu.deviceName, sizeof(g_cached_gpu.deviceName), "%s", renderer);
            } else {
                std::snprintf(g_cached_gpu.deviceName, sizeof(g_cached_gpu.deviceName), "Mali-G Series GPU (GLES 3.0 Core)");
            }
            eglMakeCurrent(instance->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }

        eglDestroySurface(instance->display, temp_surf);
        eglDestroyContext(instance->display, temp_ctx);        
        
        g_cached_gpu.display = instance->display;
        g_gpu_initialized = 1;
    }    

    pPhysicalDevices[0] = &g_cached_gpu;
    *pPhysicalDeviceCount = 1;
    return 0;
}

void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) {
    if (!physicalDevice || !pProperties) return;

    std::lock_guard<std::mutex> lock(g_gpu_init_mutex);

    std::memset(pProperties, 0, sizeof(VkPhysicalDeviceProperties));
    pProperties->apiVersion = (1 << 22) | (4 << 12); 
    pProperties->driverVersion = 1;
    pProperties->vendorID = 0x13B5; 
    pProperties->deviceID = 0x7120; 
    std::snprintf(pProperties->deviceName, sizeof(pProperties->deviceName), "%s", physicalDevice->deviceName);
}

void vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    if (!physicalDevice || !pMemoryProperties) return;

    std::memset(pMemoryProperties, 0, sizeof(VkPhysicalDeviceMemoryProperties));
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
    if (!physicalDevice || !pDevice) return -3;

    VkDevice dev = new VkDevice_T();
    dev->display = physicalDevice->display;
    
    EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8,
        EGL_NONE
    };    
    
    EGLConfig config;
    EGLint num_configs;    
    if (!eglChooseConfig(dev->display, config_attribs, &config, 1, &num_configs) || num_configs == 0) {
        delete dev;
        return -3;
    }    
    
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };    
    
    dev->context = eglCreateContext(dev->display, config, EGL_NO_CONTEXT, context_attribs);
    if (dev->context == EGL_NO_CONTEXT) {
        delete dev;
        return -3;
    }
    
    EGLint pbuffer_attribs[] = {
        EGL_WIDTH, 512,
        EGL_HEIGHT, 512,
        EGL_NONE
    };
    
    dev->surface = eglCreatePbufferSurface(dev->display, config, pbuffer_attribs);
    if (dev->surface == EGL_NO_SURFACE) {
        eglDestroyContext(dev->display, dev->context);
        delete dev;
        return -3;
    }
    
    if (!eglMakeCurrent(dev->display, dev->surface, dev->surface, dev->context)) {
        eglDestroySurface(dev->display, dev->surface);
        eglDestroyContext(dev->display, dev->context);
        delete dev;
        return -3;
    }    
    
    *pDevice = dev;
    return 0; 
}

void vkDestroyDevice(VkDevice device, const void* pAllocator) {
    if (device) {
        if (device->display != EGL_NO_DISPLAY) {
            eglMakeCurrent(device->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (device->surface != EGL_NO_SURFACE) {
                eglDestroySurface(device->display, device->surface);
            }
            if (device->context != EGL_NO_CONTEXT) {
                eglDestroyContext(device->display, device->context);
            }
        }
        delete device;
    }
}
