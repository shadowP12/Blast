#include "GfxFramebuffer.h"

namespace blast {
    GfxFramebuffer::GfxFramebuffer(const GfxFramebufferDesc& desc) {
        _width = desc.width;
        _height = desc.height;
        _num_colors = desc.num_colors;
        _has_depth_stencil = desc.has_depth_stencil;
        _sample_count = desc.sample_count;
    }
}