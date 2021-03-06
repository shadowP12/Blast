#pragma once
#include "VulkanDefine.h"
#include "../GfxSwapchain.h"
#include <vector>

namespace Blast {
    class VulkanContext;
    class VulkanTexture;
    class VulkanRenderPass;

    struct VulkanSwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VulkanSwapchain : public GfxSwapchain {
    public:
        VulkanSwapchain(VulkanContext* context, const GfxSwapchainDesc& desc);
        ~VulkanSwapchain();
        VkSwapchainKHR getHandle() { return mSwapchain; }
        uint32_t getImageCount() override;
        GfxTexture* getColorRenderTarget(uint32_t idx) override;
        GfxTexture* getDepthRenderTarget(uint32_t idx) override;
    private:
        VulkanSwapchainSupportDetails querySwapChainSupport();
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    protected:
        VulkanContext* mContext = nullptr;
        VkSurfaceKHR mSurface;
        VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
        uint32_t mImageCount = 0;
        std::vector<VulkanTexture*> mColorImages;
        std::vector<VulkanTexture*> mDepthStencilImages;
    };
}