#include "GfxSampler.h"

namespace Blast {
    GfxSampler::GfxSampler(const GfxSamplerDesc &desc) {
        mMinFilter = desc.minFilter;
        mMagFilter = desc.magFilter;
        mMipmapMode = desc.mipmapMode;
        mAddressU = desc.addressU;
        mAddressV = desc.addressV;
        mAddressW = desc.addressW;
    }
}