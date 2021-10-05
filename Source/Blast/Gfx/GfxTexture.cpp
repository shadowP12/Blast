#include "GfxTexture.h"

namespace blast {
    GfxTexture::GfxTexture(const GfxTextureDesc &desc) {
        _width = desc.width;
        _height = desc.height;
        _depth = desc.depth;
        _num_mips = desc.num_mips;
        _num_layers = desc.num_layers;
        _format = desc.format;
        _sample_count = desc.sample_count;
        _usage = desc.usage;
        _type = desc.type;
        _state = RESOURCE_STATE_UNDEFINED;
    }

    GfxTextureView::GfxTextureView(const GfxTextureViewDesc& desc) {
        _texture = desc.texture;
        _level = desc.level;
        _num_levels = desc.num_levels;
        _layer = desc.layer;
        _numd_layers = desc.num_layers;
    }
}