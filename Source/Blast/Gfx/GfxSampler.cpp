#include "GfxSampler.h"

namespace blast {
    GfxSampler::GfxSampler(const GfxSamplerDesc &desc) {
        _min_filter = desc.min_filter;
        _mag_filter = desc.mag_filter;
        _mipmap_mode = desc.mipmap_mode;
        _address_u = desc.address_u;
        _address_v = desc.address_v;
        _address_w = desc.address_w;
    }
}