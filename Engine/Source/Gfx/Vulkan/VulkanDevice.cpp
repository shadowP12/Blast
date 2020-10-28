#include "VulkanDevice.h"
#include <vector>

namespace Blast {
    static bool isLayerSupported(const char *required, const std::vector<VkLayerProperties> &available) {
        for (const VkLayerProperties &availableLayer : available) {
            if (strcmp(availableLayer.layerName, required) == 0) {
                return true;
            }
        }
        return false;
    }

    static bool isExtensionSupported(const char *required, const std::vector<VkExtensionProperties> &available) {
        for (const VkExtensionProperties &availableExtension : available) {
            if (strcmp(availableExtension.extensionName, required) == 0) {
                return true;
            }
        }
        return false;
    }

#if VULKAN_DEBUG
    VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                               VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                               const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
                                                               void *userData) {
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            BLAST_LOGW("[Vulkan]: Validation Error %s: %s\n", callbackData->pMessageIdName, callbackData->pMessage);
        } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            BLAST_LOGE("[Vulkan]: Validation Warning %s: %s\n", callbackData->pMessageIdName, callbackData->pMessage);
        }
        return VK_FALSE;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(VkDebugReportFlagsEXT flags,
                                                       VkDebugReportObjectTypeEXT type,
                                                       uint64_t object,
                                                       size_t location,
                                                       int32_t messageCode,
                                                       const char *layerPrefix,
                                                       const char *message,
                                                       void *userData) {
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            BLAST_LOGE("[Vulkan]: Validation Error: %s: %s\n", layerPrefix, message);
        } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
            BLAST_LOGW("[Vulkan]: Validation Warning: %s: %s\n", layerPrefix, message);
        } else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
            BLAST_LOGI("[Vulkan]: Validation PerfWarning: %s: %s\n", layerPrefix, message);
        } else {
            BLAST_LOGI("[Vulkan]: Validation Info: %s: %s\n", layerPrefix, message);
        }
        return VK_FALSE;
    }
#endif

    VulkanDevice::VulkanDevice() {
        if (volkInitialize() != VK_SUCCESS) {
            return;
        }

        uint32_t instanceAvailableLayerCount;
        VK_ASSERT(vkEnumerateInstanceLayerProperties(&instanceAvailableLayerCount, nullptr));
        std::vector<VkLayerProperties> instanceSupportedLayers(instanceAvailableLayerCount);
        VK_ASSERT(vkEnumerateInstanceLayerProperties(&instanceAvailableLayerCount, instanceSupportedLayers.data()));

        uint32_t instanceAvailableExtensionCount;
        VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &instanceAvailableExtensionCount, nullptr));
        std::vector<VkExtensionProperties> instanceSupportedExtensions(instanceAvailableExtensionCount);
        VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &instanceAvailableExtensionCount, instanceSupportedExtensions.data()));

        std::vector<const char*> instanceRequiredLayers;
        std::vector<const char*> instanceRequiredExtensions;
        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions;

#if VULKAN_DEBUG
        if (isLayerSupported("VK_LAYER_KHRONOS_validation", instanceSupportedLayers)) {
            instanceRequiredLayers.push_back("VK_LAYER_KHRONOS_validation");
        } else {
            instanceRequiredLayers.push_back("VK_LAYER_LUNARG_standard_validation");
        }
        bool debugUtils = false;
        if (isExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, instanceSupportedExtensions)) {
            debugUtils = true;
            instanceRequiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        } else {
            instanceRequiredExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
#endif
        instanceRequiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        instanceRequiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

        for (auto it = instanceRequiredLayers.begin(); it != instanceRequiredLayers.end(); ++it) {
            if (isLayerSupported(*it, instanceSupportedLayers)) {
                instanceLayers.push_back(*it);
            }
        }

        for (auto it = instanceRequiredExtensions.begin(); it != instanceRequiredExtensions.end(); ++it) {
            if (isExtensionSupported(*it, instanceSupportedExtensions)) {
                instanceExtensions.push_back(*it);
            }
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pEngineName = "BlastEngine";
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

#if VULKAN_DEBUG
        if (debugUtils) {
            VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
            debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            debugUtilsCreateInfo.pfnUserCallback = debugUtilsMessengerCallback;

            VK_ASSERT(vkCreateDebugUtilsMessengerEXT(mInstance, &debugUtilsCreateInfo, nullptr, &mDebugMessenger));

        } else {
            VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo{VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT};
            debugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            debugReportCreateInfo.pfnCallback = debugReportCallback;

            VK_ASSERT(vkCreateDebugReportCallbackEXT(mInstance, &debugReportCreateInfo, nullptr, &mDebugReportCallback));
        }
#endif
    }

    VulkanDevice::~VulkanDevice() {
#if VULKAN_DEBUG
        if (mDebugMessenger != VK_NULL_HANDLE) {
            vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
            mDebugMessenger = VK_NULL_HANDLE;
        }
        if (mDebugReportCallback != VK_NULL_HANDLE) {
            vkDestroyDebugReportCallbackEXT(mInstance, mDebugReportCallback, nullptr);
            mDebugReportCallback = VK_NULL_HANDLE;
        }
#endif
        vkDestroyInstance(mInstance, nullptr);
    }
}
