#pragma once
#include "GfxDefine.h"
#include <memory>
namespace Blast {
    class GfxDevice {
    public:
        uint32_t getFormatStride(Format format);
    };
}