#pragma once
#include "GfxDefine.h"

namespace Blast {
    class GfxTexture;

    struct GfxColorAttachmentDesc {
        Format format;
        SampleCount sampleCount = SAMPLE_COUNT_1;
        LoadAction loadOp = LOAD_ACTION_LOAD;
    };

    struct GfxDepthStencilAttachmentDesc {
        Format format;
        SampleCount sampleCount = SAMPLE_COUNT_1;
        LoadAction depthLoadOp = LOAD_ACTION_LOAD;
        LoadAction stencilLoadOp = LOAD_ACTION_LOAD;
    };

    struct GfxColorAttachment {
        GfxTexture* target = nullptr;
        uint32_t level = 0;
        uint32_t layer = 0;
    };

    struct GfxDepthStencilAttachment {
        GfxTexture* target = nullptr;
        uint32_t level = 0;
        uint32_t layer = 0;
    };

    struct GfxRenderPassDesc {
        uint32_t numColorAttachments = 0;
        GfxColorAttachmentDesc colors[8];
        bool hasDepthStencil = false;
        GfxDepthStencilAttachmentDesc depthStencil;
    };

    class GfxRenderPass {
    public:
        GfxRenderPass(const GfxRenderPassDesc &desc);
        uint32_t getColorAttachmentCount() { return mNumColorAttachments; }
        bool hasDepthStencilAttachment() { return mHasDepthStencil; }
    protected:
        uint32_t mNumColorAttachments = 0;
        bool mHasDepthStencil = false;
        GfxColorAttachmentDesc mColors[8];
        GfxDepthStencilAttachmentDesc mDepthStencil;
    };

    struct GfxFramebufferDesc {
        GfxRenderPass* renderPass = nullptr;
        uint32_t numColorAttachments = 0;
        GfxColorAttachment colors[8];
        bool hasDepthStencil = false;
        GfxDepthStencilAttachment depthStencil;
        uint32_t width;
        uint32_t height;
    };

    class GfxFramebuffer {
    public:
        GfxFramebuffer(const GfxFramebufferDesc &desc);
        uint32_t getWidth() { return mWidth;}
        uint32_t getHeight() { return mHeight; }
    protected:
        uint32_t mWidth;
        uint32_t mHeight;
        uint32_t mNumColorAttachments = 0;
        bool mHasDepthStencil = false;
        GfxColorAttachment mColors[8];
        GfxDepthStencilAttachment mDepthStencil;
    };
}