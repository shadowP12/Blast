#pragma once
#include "VulkanDefine.h"
#include "../GfxSwapchain.h"
#include <vector>

namespace blast {
    class VulkanContext;
    class VulkanTexture;
    class VulkanRenderPass;

    struct VulkanSwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    class VulkanSurface : public GfxSurface {
    public:
        VulkanSurface(VulkanContext* context, const GfxSurfaceDesc& desc);
        
        ~VulkanSurface();
        
        VkSurfaceKHR GetHandle() { return _surface; }
        
        VkPresentModeKHR GetPresentMode() { return _present_mode; }
        
        VkSurfaceFormatKHR GetSurfaceFormat() { return _surface_format; }
        
        VkSurfaceCapabilitiesKHR GetCapabilities();
        
    private:
        VulkanSwapchainSupportDetails QuerySwapChainSupport(VkSurfaceKHR surface);
        
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
        
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
        
    private:
        VulkanContext* _context = nullptr;
        VkSurfaceKHR _surface;
        VkPresentModeKHR _present_mode;
        VkSurfaceFormatKHR _surface_format;
    };

    class VulkanSwapchain : public GfxSwapchain {
    public:
        VulkanSwapchain(VulkanContext* context, const GfxSwapchainDesc& desc);

        ~VulkanSwapchain();

        VkSwapchainKHR GetHandle() { return _swapchain; }

        uint32_t GetImageCount() override;

        GfxTexture* GetColorRenderTarget(uint32_t idx) override;

        GfxTexture* GetDepthRenderTarget(uint32_t idx) override;

    protected:
        VulkanContext* _context = nullptr;
        VkSwapchainKHR _swapchain;
        uint32_t _num_images = 0;
        std::vector<VulkanTexture*> _color_images;
        std::vector<VulkanTexture*> _depth_stencil_images;
    };
}