#pragma once
#include "GfxDefine.h"

namespace Blast {
    struct GfxTextureDesc {
        uint32_t width;
        uint32_t height;
        uint32_t depth = 1;
        uint32_t mipLevels = 1;
        uint32_t arrayLayers = 1;
        bool colorAtt = false;
        bool depthStencilAtt = false;
        Format format;
        SampleCount sampleCount = SAMPLE_COUNT_1;
        ResourceUsage usage;
        ResourceType type;
        ResourceState state;
    };

    class GfxTexture {
    public:
        GfxTexture(const GfxTextureDesc& desc);
    protected:
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mDepth;
        uint32_t mMipLevels;
        uint32_t mArrayLayers;
        Format mFormat;
        SampleCount mSampleCount;
        ResourceUsage mUsage;
        ResourceType mType;
        ResourceState mState;
    };
}
