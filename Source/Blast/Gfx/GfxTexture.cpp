#include "GfxTexture.h"

namespace Blast {
    GfxTexture::GfxTexture(const GfxTextureDesc &desc) {
        mWidth = desc.width;
        mHeight = desc.height;
        mDepth = desc.depth;
        mMipLevels = desc.mipLevels;
        mArrayLayers = desc.arrayLayers;
        mFormat = desc.format;
        mSampleCount = desc.sampleCount;
        mUsage = desc.usage;
        mType = desc.type;
        mState = RESOURCE_STATE_UNDEFINED;
    }
}