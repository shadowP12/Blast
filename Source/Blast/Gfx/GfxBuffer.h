#pragma once
#include "GfxDefine.h"

namespace blast {
    struct GfxBufferDesc {
        uint32_t size;
        ResourceUsage usage;
        ResourceType type;
    };

    class GfxBuffer {
    public:
        GfxBuffer(const GfxBufferDesc &desc);

        virtual ~GfxBuffer() = default;

        virtual void ReadData(uint32_t offset, uint32_t size, void* dest) = 0;

        virtual void WriteData(uint32_t offset, uint32_t size, void* dest) = 0;

        ResourceType GetResourceType() { return _type; }

        ResourceState GetResourceState() { return _state; }

        void SetResourceState(ResourceState state) { _state = state; }

    protected:
        uint32_t _size;
        ResourceUsage _usage;
        ResourceType _type;
        ResourceState _state;
    };
}