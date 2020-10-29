#pragma once
#include "VulkanDefine.h"
#include "../GfxBuffer.h"

namespace Blast {
    class VulkanBuffer : public GfxBuffe {
    public:
        VulkanBuffer(GfxDevice *device, const GfxBufferDesc &desc);
        virtual ~VulkanBuffer();
    };
}