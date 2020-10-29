#include "VulkanBuffer.h"
#include "../GfxDevice.h"
#include "VulkanDevice.h"

namespace Blast {
    VulkanBuffer::VulkanBuffer(GfxDevice *device, const GfxBufferDesc &desc)
    : GfxBuffe(device, desc) {
        VulkanDevice* internalDevice = static_cast<VulkanDevice*>(device);
    }

    VulkanBuffer::~VulkanBuffer() {
    }
}