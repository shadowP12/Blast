#include "GfxRenderTarget.h"

namespace Blast {
    GfxRenderTarget::GfxRenderTarget(const GfxRenderTargetDesc &desc) {
        for (int i = 0; i < 8; ++i) {
            mColor[i] = desc.color[i];
        }
        mDepthStencil = desc.depthStencil;
        mNumColorAttachments = desc.numColorAttachments;
        mHasDepth = desc.hasDepth;
        mUsage = desc.usage;
        mType = desc.type;
        mState = desc.state;
    }
}