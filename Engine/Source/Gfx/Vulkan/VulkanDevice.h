#pragma once
#include "../GfxDevice.h"
#include "VulkanDefine.h"

namespace Blast {
    class VulkanDevice : public GfxDevice {
    public:
        VulkanDevice();
        VkDevice getHandle() { return mDevice; }
        VkPhysicalDevice getPhyDevice() { return mPhyDevice; }
        VkInstance getInstance() { return mInstance; }
        int findMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties);
        virtual ~VulkanDevice();
        virtual GfxBuffer* createBuffer(const GfxBufferDesc& desc);
    private:
        VkInstance mInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT mDebugReportCallback = VK_NULL_HANDLE;
        VkPhysicalDevice mPhyDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties mPhyDeviceProperties;
        VkPhysicalDeviceFeatures mPhyDeviceFeatures;
        VkPhysicalDeviceMemoryProperties mPhyDeviceMemoryProperties;
        VkDevice mDevice = VK_NULL_HANDLE;
    };
}