#pragma once
#include "GfxDefine.h"

namespace Blast {
    class GfxTexture;

    struct GfxColorAttachmentInfo {
        GfxTexture* texture = nullptr;
        LoadAction loadOp = LOAD_ACTION_LOAD;
    };

    struct GfxDepthStencilAttachmentInfo {
        GfxTexture* texture = nullptr;
        LoadAction depthLoadOp = LOAD_ACTION_LOAD;
        LoadAction stencilLoadOp = LOAD_ACTION_LOAD;
    };

    struct GfxRenderTargetDesc {
        uint32_t numColorAttachments = 0;
        bool hasDepth = false;
        GfxColorAttachmentInfo color[8];
        GfxDepthStencilAttachmentInfo depthStencil;
        ResourceUsage usage;
        ResourceType type;
        ResourceState state;
    };

    class GfxRenderTarget {
    public:
        GfxRenderTarget(const GfxRenderTargetDesc &desc);
        uint32_t getColorAttachmentCount() { return mNumColorAttachments; }
    protected:
        uint32_t mNumColorAttachments = 0;
        bool mHasDepth = false;
        GfxColorAttachmentInfo mColor[8];
        GfxDepthStencilAttachmentInfo mDepthStencil;
        ResourceUsage mUsage;
        ResourceType mType;
        ResourceState mState;
    };
}