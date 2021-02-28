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

    struct GfxRenderPassDesc {
        uint32_t numColorAttachments = 0;
        bool hasDepth = false;
        GfxColorAttachmentInfo color[8];
        GfxDepthStencilAttachmentInfo depthStencil;
        ResourceUsage usage;
        ResourceType type;
        ResourceState state;
    };

    class GfxRenderPass {
    public:
        GfxRenderPass(const GfxRenderPassDesc &desc);
        uint32_t getColorAttachmentCount() { return mNumColorAttachments; }
        virtual GfxTexture* getColorRT(int idx) = 0;
        virtual GfxTexture* getDepthRT() = 0;
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