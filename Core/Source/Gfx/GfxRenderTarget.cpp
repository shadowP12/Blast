#include "GfxRenderTarget.h"

namespace Blast {
    GfxRenderTarget::GfxRenderTarget(const GfxRenderTargetDesc &desc) {
        mNumColorAttachments = desc.numColorAttachments;
        mHasDepth = desc.hasDepth;
        for (int i = 0; i < 8; ++i) {
            mColor[i] = desc.color[i];
        }
        mDepthStencil = desc.depthStencil;
        mUsage = desc.usage;
        mType = desc.type;
        mState = desc.state;
    }
}