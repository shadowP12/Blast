#pragma once
#include "VulkanDefine.h"
#include "../GfxSwapchain.h"
#include <vector>

namespace Blast {
    class VulkanContext;
    class VulkanTexture;
    class VulkanRenderTarget;

    struct VulkanSwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VulkanSwapchain : public GfxSwapchain {
    public:
        VulkanSwapchain(VulkanContext* context, const GfxSwapchainDesc& desc);
        virtual ~VulkanSwapchain();
        VkSwapchainKHR getHandle() { return mSwapchain; }
        GfxRenderTarget* getRenderTarget(int idx) override;
    private:
        VulkanSwapchainSupportDetails querySwapChainSupport();
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    protected:
        VulkanContext* mContext = nullptr;
        VkSurfaceKHR mSurface;
        VkSwapchainKHR mSwapchain;
        std::vector<VulkanTexture*> mColorImages;
        std::vector<VulkanTexture*> mDepthStencilImages;
        std::vector<VulkanRenderTarget*> mRenderTargets;
    };
}