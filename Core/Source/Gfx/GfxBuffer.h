#pragma once
#include "GfxDefine.h"

namespace Blast {
    struct GfxBufferDesc {
        uint32_t size;
        ResourceUsage usage;
        ResourceType type;
    };

    class GfxBuffe {
    public:
        GfxBuffe(const GfxBufferDesc &desc);
        virtual void readData(uint32_t offset, uint32_t size, void * dest) = 0;
        virtual void writeData(uint32_t offset, uint32_t size, void * dest) = 0;
    protected:
        uint32_t mSize;
        ResourceUsage mUsage;
        ResourceType mType;
        ResourceState mState;
    };
}