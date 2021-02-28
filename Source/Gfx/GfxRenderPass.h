#pragma once
#include "GfxDefine.h"

namespace Blast {
    class GfxTexture;

    struct GfxColorAttachmentInfo {
        Format format;
        SampleCount sampleCount = SAMPLE_COUNT_1;
        LoadAction loadOp = LOAD_ACTION_LOAD;
    };

    struct GfxDepthStencilAttachmentInfo {
        Format format;
        SampleCount sampleCount = SAMPLE_COUNT_1;
        LoadAction depthLoadOp = LOAD_ACTION_LOAD;
        LoadAction stencilLoadOp = LOAD_ACTION_LOAD;
    };

    struct GfxRenderPassDesc {
        uint32_t numColorAttachments = 0;
        GfxColorAttachmentInfo color[8];
        bool hasDepth = false;
        GfxDepthStencilAttachmentInfo depthStencil;
    };

    class GfxRenderPass {
    public:
        GfxRenderPass(const GfxRenderPassDesc &desc);
        uint32_t getColorAttachmentCount() { return mNumColorAttachments; }
    protected:
        uint32_t mNumColorAttachments = 0;
        bool mHasDepth = false;
        GfxColorAttachmentInfo mColor[8];
        GfxDepthStencilAttachmentInfo mDepthStencil;
    };

    struct GfxFramebufferDesc {
        GfxRenderPass* renderPass;
        uint32_t colorCount;
        GfxTexture* colors[8];
        bool hasDepth;
        GfxTexture* depth;
        uint32_t width;
        uint32_t height;
    };

    class GfxFramebuffer {
    public:
        GfxFramebuffer(const GfxFramebufferDesc& desc);
        uint32_t getWidth() { return mWidth;}
        uint32_t getHeight() { return mHeight; }
        virtual GfxRenderPass* getRenderPass() { return mRenderPass; }
    protected:
        uint32_t mWidth;
        uint32_t mHeight;
        GfxRenderPass* mRenderPass;
    };


}