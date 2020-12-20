#include "VulkanDevice.h"
#include "VulkanBuffer.h"
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
        uint32_t gpuCount = 0;
        VK_ASSERT(vkEnumeratePhysicalDevices(mInstance, &gpuCount, nullptr));

        std::vector<VkPhysicalDevice> gpus(gpuCount);
        VK_ASSERT(vkEnumeratePhysicalDevices(mInstance, &gpuCount, gpus.data()));

        for (auto &g : gpus) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(g, &props);
            BLAST_LOGI("Found Vulkan GPU: %s\n", props.deviceName);
            BLAST_LOGI("API: %u.%u.%u\n",
                   VK_VERSION_MAJOR(props.apiVersion),
                   VK_VERSION_MINOR(props.apiVersion),
                   VK_VERSION_PATCH(props.apiVersion));
            BLAST_LOGI("Driver: %u.%u.%u\n",
                   VK_VERSION_MAJOR(props.driverVersion),
                   VK_VERSION_MINOR(props.driverVersion),
                   VK_VERSION_PATCH(props.driverVersion));
        }

        // todo
        mPhyDevice = gpus.front();

        vkGetPhysicalDeviceProperties(mPhyDevice, &mPhyDeviceProperties);
        vkGetPhysicalDeviceFeatures(mPhyDevice, &mPhyDeviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(mPhyDevice, &mPhyDeviceMemoryProperties);

        uint32_t queueFamiliesCount;
        vkGetPhysicalDeviceQueueFamilyProperties(mPhyDevice, &queueFamiliesCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamiliesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(mPhyDevice, &queueFamiliesCount, queueFamilyProperties.data());

        uint32_t graphicsFamily = -1;
        uint32_t computeFamily = -1;
        uint32_t transferFamily = -1;

        for (uint32_t i = 0; i < (uint32_t)queueFamilyProperties.size(); i++) {
            if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
                computeFamily = i;
                break;
            }
        }

        for (uint32_t i = 0; i < (uint32_t)queueFamilyProperties.size(); i++) {
            if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
                ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) {
                transferFamily = i;
                break;
            }
        }

        for (uint32_t i = 0; i < (uint32_t)queueFamilyProperties.size(); i++) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsFamily = i;
                break;
            }
        }

        const float graphicsQueuePrio = 0.0f;
        const float computeQueuePrio = 0.1f;
        const float transferQueuePrio = 0.2f;

        std::vector<VkDeviceQueueCreateInfo> queueInfo{};
        queueInfo.resize(3);

        queueInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo[0].queueFamilyIndex = graphicsFamily;
        queueInfo[0].queueCount = 1;
        queueInfo[0].pQueuePriorities = &graphicsQueuePrio;

        queueInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo[1].queueFamilyIndex = computeFamily;
        queueInfo[1].queueCount = 1;
        queueInfo[1].pQueuePriorities = &computeQueuePrio;

        queueInfo[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo[2].queueFamilyIndex = transferFamily;
        queueInfo[2].queueCount = 1;
        queueInfo[2].pQueuePriorities = &transferQueuePrio;

        uint32_t deviceAvailableExtensionCount = 0;
        VK_ASSERT(vkEnumerateDeviceExtensionProperties(mPhyDevice, nullptr, &deviceAvailableExtensionCount, nullptr));

        std::vector<VkExtensionProperties> deviceAvailableExtensions(deviceAvailableExtensionCount);
        VK_ASSERT(vkEnumerateDeviceExtensionProperties(mPhyDevice, nullptr, &deviceAvailableExtensionCount, deviceAvailableExtensions.data()));

        std::vector<const char*> deviceRequiredExtensions;
        std::vector<const char*> deviceExtensions;

        deviceRequiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        deviceRequiredExtensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
        deviceRequiredExtensions.push_back(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
        deviceRequiredExtensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
        deviceRequiredExtensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);

        for (auto it = deviceExtensions.begin(); it != deviceExtensions.end(); ++it) {
            if (isExtensionSupported(*it, deviceAvailableExtensions)) {
                deviceExtensions.push_back(*it);
            }
        }

        VkDeviceCreateInfo deviceInfo ;
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.pNext = nullptr;
        deviceInfo.flags = 0;
        deviceInfo.queueCreateInfoCount = queueInfo.size();
        deviceInfo.pQueueCreateInfos = queueInfo.data();
        deviceInfo.pEnabledFeatures = &mPhyDeviceFeatures;
        deviceInfo.enabledExtensionCount = deviceExtensions.size();
        deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceInfo.enabledLayerCount = 0;
        deviceInfo.ppEnabledLayerNames = nullptr;

        VK_ASSERT(vkCreateDevice(mPhyDevice, &deviceInfo, nullptr, &mDevice));
    }

    VulkanDevice::~VulkanDevice() {
        vkDeviceWaitIdle(mDevice);
        vkDestroyDevice(mDevice, nullptr);

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

    int VulkanDevice::findMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties) {
        for (int i = 0; i < mPhyDeviceMemoryProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (mPhyDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        return -1;
    }

    GfxBuffer* VulkanDevice::createBuffer(const GfxBufferDesc& desc) {
        VulkanBuffer* buf = new VulkanBuffer(this, desc);
        return (GfxBuffer*)buf;
    }
}
