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

}
