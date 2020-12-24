#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#if WIN32
#include <windows.h>
#endif

namespace Blast {
    VulkanSwapchain::VulkanSwapchain(VulkanDevice *device, const GfxSwapchainDesc &desc)
    :GfxSwapchain(desc){
        mDevice = device;

        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
#if WIN32
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = NULL;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = ::GetModuleHandle(NULL);
        surfaceInfo.hwnd = (HWND)desc.windowHandle;
        VK_ASSERT(vkCreateWin32SurfaceKHR(mDevice->getInstance(), &surfaceInfo, nullptr, &mSurface));
#endif
    }

    VulkanSwapchain::~VulkanSwapchain() {

    }

    GfxRenderTarget* VulkanSwapchain::getRenderTarget(int idx) {

    }
}