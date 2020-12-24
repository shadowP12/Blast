#pragma once
#include "VulkanDefine.h"
#include "../GfxSwapchain.h"
#include <vector>

namespace Blast {
    class VulkanDevice;
    class VulkanRenderTarget;

    class VulkanSwapchain : public GfxSwapchain {
    public:
        VulkanSwapchain(VulkanDevice *device, const GfxSwapchainDesc &desc);
        virtual ~VulkanSwapchain();
        GfxRenderTarget* getRenderTarget(int idx) override;
    protected:
        VulkanDevice* mDevice = nullptr;
        VkSurfaceKHR mSurface;
        std::vector<VulkanRenderTarget*> mRenderTargets;
    };
}