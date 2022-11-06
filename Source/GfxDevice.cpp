#include "GfxDevice.h"

// Vulkan
#include <Vulkan/VulkanDevice.h>

namespace blast {
    GfxDevice* GfxDevice::CreateDevice() {
        return new VulkanDevice();
    }
}