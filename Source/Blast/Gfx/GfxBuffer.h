#pragma once
#include "GfxDefine.h"

namespace Blast {
    struct GfxBufferDesc {
        uint32_t size;
        ResourceUsage usage;
        ResourceType type;
    };

    class GfxBuffer {
    public:
        GfxBuffer(const GfxBufferDesc &desc);
        virtual ~GfxBuffer() = default;
        virtual void readData(uint32_t offset, uint32_t size, void * dest) = 0;
        virtual void writeData(uint32_t offset, uint32_t size, void * dest) = 0;
        ResourceType getResourceType() { return mType; }
        ResourceState getResourceState() { return mState; }
        void setResourceState(ResourceState state) { mState = state; }
    protected:
        uint32_t mSize;
        ResourceUsage mUsage;
        ResourceType mType;
        ResourceState mState;
    };
}