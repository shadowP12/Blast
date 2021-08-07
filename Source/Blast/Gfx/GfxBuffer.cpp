#include "GfxBuffer.h"

namespace blast {
    GfxBuffer::GfxBuffer(const GfxBufferDesc &desc) {
        _size = desc.size;
        _usage = desc.usage;
        _type = desc.type;
        _state = RESOURCE_STATE_UNDEFINED;
    }
}