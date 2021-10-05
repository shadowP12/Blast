#include "GfxFramebuffer.h"

namespace blast {
    GfxFramebuffer::GfxFramebuffer(const GfxFramebufferDesc& desc) {
        _width = desc.width;
        _height = desc.height;
        _num_colors = desc.num_colors;
        _has_depth_stencil = desc.has_depth_stencil;
        _sample_count = desc.sample_count;
        _depth_stencil = desc.depth_stencil;
        for (uint32_t i = 0; i < _num_colors; i++) {
            _colors[i] = desc.colors[i];
        }
    }

    GfxTextureView* GfxFramebuffer::GetColor(uint32_t index) {
        return _colors[index];
    }

    GfxTextureView* GfxFramebuffer::GetDepthStencil() {
        return _depth_stencil;
    }
}