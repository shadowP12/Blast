#pragma once
#include "GfxDefine.h"

namespace Blast {
    class GfxRenderPass;
    class GfxTexture;

    struct GfxSwapchainDesc {
        void* windowHandle;
        uint32_t width;
        uint32_t height;
    };

    class GfxSwapchain {
    public:
        GfxSwapchain(const GfxSwapchainDesc& desc);
        virtual uint32_t getImageCount() = 0;
        virtual GfxTexture* getColorRenderTarget(uint32_t idx) = 0;
        virtual GfxTexture* getDepthRenderTarget(uint32_t idx) = 0;
    protected:
        uint32_t mWidth;
        uint32_t mHeight;
    };

}