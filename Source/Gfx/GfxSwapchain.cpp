#include "GfxSwapchain.h"

namespace Blast {
    GfxSurface::GfxSurface(const GfxSurfaceDesc& desc) {
        mOriginSurface = desc.originSurface;
    }

    GfxSwapchain::GfxSwapchain(const GfxSwapchainDesc &desc) {
        mWidth = desc.width;
        mHeight = desc.height;
    }
}