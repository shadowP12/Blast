#include "GfxSwapchain.h"
#if WIN32
#include <windows.h>
#endif

namespace Blast {
    GfxSurface::GfxSurface(const GfxSurfaceDesc& desc) {
        mOriginSurface = desc.originSurface;
    }

    GfxSurfaceSize GfxSurface::getSize() {
        GfxSurfaceSize size;
#if WIN32
        RECT rect;
        GetClientRect((HWND)mOriginSurface, (LPRECT)&rect);
        size.width = rect.right - rect.left;
        size.height = rect.bottom - rect.top;
#endif
        return size;
    }

    GfxSwapchain::GfxSwapchain(const GfxSwapchainDesc &desc) {
        mWidth = desc.width;
        mHeight = desc.height;
    }
}