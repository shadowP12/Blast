#pragma once
#include "GfxDefine.h"

namespace Blast {
    class GfxRenderPass;
    class GfxTexture;
    class GfxSwapchain;

    struct GfxSurfaceDesc {
        void* originSurface = nullptr;
    };

    class GfxSurface {
    public:
        GfxSurface(const GfxSurfaceDesc& desc);
        Format getFormat() { return mFormat; }
    protected:
        void* mOriginSurface = nullptr;
        Format mFormat;
    };

    struct GfxSwapchainDesc {
        GfxSurface* surface = nullptr;
        GfxSwapchain* oldSwapchain = nullptr;
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