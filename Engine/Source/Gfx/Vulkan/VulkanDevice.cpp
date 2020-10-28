#include "VulkanDevice.h"
#include <vector>

namespace Blast {
    VulkanDevice::VulkanDevice() {
        if (volkInitialize() != VK_SUCCESS) {
            return;
        }

        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions;
#if VULKAN_DEBUG
        instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "GearEngine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "GearEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceInfo;
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pNext = nullptr;
        instanceInfo.flags = 0;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledLayerCount = instanceLayers.size();
        instanceInfo.ppEnabledLayerNames = instanceLayers.data();
        instanceInfo.enabledExtensionCount = instanceExtensions.size();
        instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

        VK_ASSERT(vkCreateInstance(&instanceInfo, nullptr, &mInstance));

        volkLoadInstance(mInstance);
    }

    VulkanDevice::~VulkanDevice() {
        vkDestroyInstance(mInstance, nullptr);
    }
}
