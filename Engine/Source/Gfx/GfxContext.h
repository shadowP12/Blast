#pragma once
#include "GfxDefine.h"
#include <memory>
namespace Blast {

    struct GfxBufferDesc;
    class GfxBuffer;

    class GfxContext {
    public:
        uint32_t getFormatStride(Format format);
        virtual GfxBuffer* createBuffer(const GfxBufferDesc& desc) { return nullptr; }
    };
}