#include "GfxSwapchain.h"

namespace Blast {
    GfxSwapchain::GfxSwapchain(const GfxSwapchainDesc &desc) {
        mWidth = desc.width;
        mHeight = desc.height;
    }
}