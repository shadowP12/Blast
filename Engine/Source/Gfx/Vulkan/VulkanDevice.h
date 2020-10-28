#pragma once
#include "../GfxDevice.h"
#include "VulkanDefine.h"

namespace Blast {
    class VulkanDevice : public GfxDevice {
    public:
        VulkanDevice();
        virtual ~VulkanDevice();

    private:
        VkInstance mInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT mDebugReportCallback = VK_NULL_HANDLE;
    };
}