#pragma once
#include "GfxDefine.h"

namespace Blast {
    struct GfxSamplerDesc {
        FilterType  minFilter;
        FilterType  magFilter;
        MipmapMode  mipmapMode;
        AddressMode addressU;
        AddressMode addressV;
        AddressMode addressW;
    };

    class GfxSampler {
    public:
        GfxSampler(const GfxSamplerDesc &desc);
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