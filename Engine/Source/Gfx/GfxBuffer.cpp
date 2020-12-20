#include "GfxBuffer.h"

namespace Blast {
    GfxBuffe::GfxBuffe(const GfxBufferDesc &desc) {
        mSize = desc.size;
        mUsage = desc.usage;
        mType = desc.type;
        mState = RESOURCE_STATE_UNDEFINED;
    }
}