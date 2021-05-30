#pragma once
#include "GfxDefine.h"

namespace Blast {
    struct GfxSamplerDesc {
        FilterType  minFilter = FILTER_LINEAR;
        FilterType  magFilter = FILTER_LINEAR;
        MipmapMode  mipmapMode = MIPMAP_MODE_LINEAR;
        AddressMode addressU = ADDRESS_MODE_REPEAT;
        AddressMode addressV = ADDRESS_MODE_REPEAT;
        AddressMode addressW = ADDRESS_MODE_REPEAT;
    };

    class GfxSampler {
    public:
        GfxSampler(const GfxSamplerDesc &desc);
        virtual ~GfxSampler() = default;
        FilterType  getMinFilter() { mMinFilter; }
        FilterType  getMagFilter() { mMagFilter; }
        MipmapMode  getMipmapMode() { mMipmapMode; }
        AddressMode getAddressU() { mAddressU; }
        AddressMode getAddressV() { mAddressV; }
        AddressMode getAddressW() { mAddressW; }
    protected:
        FilterType  mMinFilter;
        FilterType  mMagFilter;
        MipmapMode  mMipmapMode;
        AddressMode mAddressU;
        AddressMode mAddressV;
        AddressMode mAddressW;
    };
}