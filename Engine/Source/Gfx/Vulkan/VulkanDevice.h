#pragma once
#include "../GfxDevice.h"
namespace Blast {
    class VulkanDevice : public GfxDevice {
    public:
        VulkanDevice();
        virtual ~VulkanDevice();
    };
}
