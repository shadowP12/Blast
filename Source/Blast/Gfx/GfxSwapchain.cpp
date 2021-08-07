#include "GfxSwapchain.h"

namespace blast {
    GfxSurface::GfxSurface(const GfxSurfaceDesc& desc) {
        _origin_surface = desc.origin_surface;
    }

    GfxSwapchain::GfxSwapchain(const GfxSwapchainDesc &desc) {
        _width = desc.width;
        _height = desc.height;
    }
}