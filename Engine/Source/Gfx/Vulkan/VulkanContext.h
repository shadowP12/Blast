#pragma once
#include "../GfxContext.h"
#include "VulkanDefine.h"

namespace Blast {
    class VulkanContext : public GfxContext {
    public:
        VulkanContext();
        virtual ~VulkanContext();
        VkDevice getDevice() { return mDevice; }
        VkPhysicalDevice getPhyDevice() { return mPhyDevice; }
        VkInstance getInstance() { return mInstance; }
        uint32_t getGraphicsFamily() { return mGraphicsFamily; }
        uint32_t getComputeFamily() { return mComputeFamily; }
        uint32_t getTransferFamily() { return mTransferFamily; }
        int findMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties);
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
        uint32_t mGraphicsFamily = -1;
        uint32_t mComputeFamily = -1;
        uint32_t mTransferFamily = -1;
    };
}