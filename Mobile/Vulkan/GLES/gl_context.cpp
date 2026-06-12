#include <EGL/egl.h>
#include <GLES3/gl31.h>
#include <vulkan/vulkan.h>

EGLDisplay global_display = EGL_NO_DISPLAY;
EGLContext global_context = EGL_NO_CONTEXT;
EGLConfig  global_config;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    global_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (global_display == EGL_NO_DISPLAY) return VK_ERROR_INITIALIZATION_FAILED;

    if (!eglInitialize(global_display, nullptr, nullptr)) return VK_ERROR_INITIALIZATION_FAILED;

    *pInstance = (VkInstance)&global_display;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    const EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    EGLint numConfigs;
    eglChooseConfig(global_display, configAttribs, &global_config, 1, &numConfigs);

    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    global_context = eglCreateContext(global_display, global_config, EGL_NO_CONTEXT, contextAttribs);

    if (global_context == EGL_NO_CONTEXT) {
        contextAttribs[1] = 3;
        global_context = eglCreateContext(global_display, global_config, EGL_NO_CONTEXT, contextAttribs);
        if (global_context == EGL_NO_CONTEXT) return VK_ERROR_INITIALIZATION_FAILED;
    }

    *pDevice = (VkDevice)&global_context;
    return VK_SUCCESS;
}

VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
    if (pPhysicalDevices) *pPhysicalDevices = (VkPhysicalDevice)1;
    *pPhysicalDeviceCount = 1;
    return VK_SUCCESS;
}
