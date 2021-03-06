#include "GfxRenderPass.h"

namespace Blast {
    GfxRenderPass::GfxRenderPass(const GfxRenderPassDesc &desc) {
        mNumColorAttachments = desc.numColorAttachments;
        for (int i = 0; i < 8; ++i) {
            mColors[i] = desc.colors[i];
        }
        mHasDepthStencil = desc.hasDepthStencil;
        mDepthStencil = desc.depthStencil;
        mWidth = desc.width;
        mHeight = desc.height;
    }
}