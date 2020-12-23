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
        GfxColorAttachmentInfo color[8];
        GfxDepthStencilAttachmentInfo depthStencil;
        uint32_t numColorAttachments = 0;
        bool hasDepth = false;
        ResourceUsage usage;
        ResourceType type;
        ResourceState state;
    };

    class GfxRenderTarget {
    public:
        GfxRenderTarget(const GfxRenderTargetDesc &desc);
    protected:
        GfxColorAttachmentInfo mColor[8];
        GfxDepthStencilAttachmentInfo mDepthStencil;
        uint32_t mNumColorAttachments = 0;
        bool mHasDepth = false;
        ResourceUsage mUsage;
        ResourceType mType;
        ResourceState mState;
    };
}