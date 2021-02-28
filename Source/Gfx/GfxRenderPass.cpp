#include "GfxRenderPass.h"

namespace Blast {
    GfxRenderPass::GfxRenderPass(const GfxRenderPassDesc &desc) {
        mNumColorAttachments = desc.numColorAttachments;
        mHasDepth = desc.hasDepth;
        for (int i = 0; i < 8; ++i) {
            mColor[i] = desc.color[i];
        }
        mDepthStencil = desc.depthStencil;
    }

    GfxFramebuffer::GfxFramebuffer(const GfxFramebufferDesc& desc) {
        mWidth = desc.width;
        mHeight = desc.height;
        mRenderPass = desc.renderPass;
    }
}