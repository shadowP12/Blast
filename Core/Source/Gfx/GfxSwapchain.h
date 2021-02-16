#pragma once
#include "GfxDefine.h"

namespace Blast {
    class GfxRenderTarget;

    struct GfxSwapchainDesc {
        void* windowHandle;
        uint32_t width;
        uint32_t height;
    };

    class GfxSwapchain {
    public:
        GfxSwapchain(const GfxSwapchainDesc& desc);
        virtual GfxRenderTarget* getRenderTarget(int idx) = 0;
    protected:
        uint32_t mWidth;
        uint32_t mHeight;
    };

}