#pragma once
#include "GfxDefine.h"

namespace Blast {
    struct GfxBufferDesc {
        uint32_t size;
        ResourceUsage usage;
        ResourceType type;
    };

    class GfxDevice;
    class GfxBuffe {
    public:
        GfxBuffe(GfxDevice *device, const GfxBufferDesc &desc) {}
    };
}