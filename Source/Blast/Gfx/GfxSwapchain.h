#pragma once
#include "GfxDefine.h"

namespace blast {
    class GfxRenderPass;
    class GfxTexture;
    class GfxSwapchain;

    struct GfxSurfaceSize {
        uint32_t width;
        uint32_t height;
    };

    struct GfxSurfaceDesc {
        void* origin_surface = nullptr;
    };

    class GfxSurface {
    public:
        GfxSurface(const GfxSurfaceDesc& desc);

        virtual ~GfxSurface() = default;

    protected:
        void* _origin_surface = nullptr;
    };

    struct GfxSwapchainDesc {
        GfxSurface* surface = nullptr;
        GfxSwapchain* old_swapchain = nullptr;
        uint32_t width;
        uint32_t height;
    };

    class GfxSwapchain {
    public:
        GfxSwapchain(const GfxSwapchainDesc& desc);

        virtual ~GfxSwapchain() = default;

        virtual uint32_t GetImageCount() = 0;

        virtual GfxTexture* GetColorRenderTarget(uint32_t idx) = 0;

        virtual GfxTexture* GetDepthRenderTarget(uint32_t idx) = 0;

    protected:
        uint32_t _width;
        uint32_t _height;
    };

}