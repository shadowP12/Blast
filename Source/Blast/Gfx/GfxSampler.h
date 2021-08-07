#pragma once
#include "GfxDefine.h"

namespace blast {
    struct GfxSamplerDesc {
        FilterType  min_filter = FILTER_LINEAR;
        FilterType  mag_filter = FILTER_LINEAR;
        MipmapMode  mipmap_mode = MIPMAP_MODE_LINEAR;
        AddressMode address_u = ADDRESS_MODE_REPEAT;
        AddressMode address_v = ADDRESS_MODE_REPEAT;
        AddressMode address_w = ADDRESS_MODE_REPEAT;
    };

    class GfxSampler {
    public:
        GfxSampler(const GfxSamplerDesc &desc);

        virtual ~GfxSampler() = default;

        FilterType  GetMinFilter() { _min_filter; }

        FilterType  GetMagFilter() { _mag_filter; }

        MipmapMode  GetMipmapMode() { _mipmap_mode; }

        AddressMode GetAddressU() { _address_u; }

        AddressMode GetAddressV() { _address_v; }

        AddressMode GetAddressW() { _address_w; }

    protected:
        FilterType  _min_filter;
        FilterType  _mag_filter;
        MipmapMode  _mipmap_mode;
        AddressMode _address_u;
        AddressMode _address_v;
        AddressMode _address_w;
    };
}