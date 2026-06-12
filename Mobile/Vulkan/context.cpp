#include <EGL/egl.h>
#include <GLES3/gl31.h>
#include <string.h>

typedef void* VkInstance;
typedef void* VkPhysicalDevice;
typedef void* VkDevice;
typedef uint32_t VkResult;
typedef void (*PFN_vkVoidFunction)(void);

#define VK_SUCCESS 0
#define VK_ERROR_INITIALIZATION_FAILED -3

EGLDisplay global_display = EGL_NO_DISPLAY;
EGLContext global_context = EGL_NO_CONTEXT;
EGLConfig  global_config;

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

PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    if (strcmp(pName, "vkCreateInstance") == 0) return (PFN_vkVoidFunction)vkCreateInstance;
    if (strcmp(pName, "vkCreateDevice") == 0) return (PFN_vkVoidFunction)vkCreateDevice;
    return nullptr;
}

}
