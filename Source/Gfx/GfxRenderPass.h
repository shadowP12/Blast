#pragma once
#include "GfxDefine.h"

namespace Blast {
    class GfxTexture;

    struct GfxColorAttachmentInfo {
        GfxTexture* target = nullptr;
        uint32_t level = 0;
        uint32_t layer = 0;
        LoadAction loadOp = LOAD_ACTION_LOAD;
    };

    struct GfxDepthStencilAttachmentInfo {
        GfxTexture* target = nullptr;
        uint32_t level = 0;
        uint32_t layer = 0;
        LoadAction depthLoadOp = LOAD_ACTION_LOAD;
        LoadAction stencilLoadOp = LOAD_ACTION_LOAD;
    };

    struct GfxRenderPassDesc {
        uint32_t numColorAttachments = 0;
        GfxColorAttachmentInfo colors[8];
        bool hasDepthStencil = false;
        GfxDepthStencilAttachmentInfo depthStencil;
        uint32_t width;
        uint32_t height;
    };

    class GfxRenderPass {
    public:
        GfxRenderPass(const GfxRenderPassDesc &desc);
        uint32_t getWidth() { return mWidth;}
        uint32_t getHeight() { return mHeight; }
        uint32_t getColorAttachmentCount() { return mNumColorAttachments; }
        bool hasDepthStencilAttachment() { return mHasDepthStencil; }
    protected:
        uint32_t mNumColorAttachments = 0;
        bool mHasDepthStencil = false;
        GfxColorAttachmentInfo mColors[8];
        GfxDepthStencilAttachmentInfo mDepthStencil;
        uint32_t mWidth;
        uint32_t mHeight;
    };
}