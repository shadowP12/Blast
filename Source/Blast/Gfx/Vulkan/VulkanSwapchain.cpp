#include "VulkanSwapchain.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#if WIN32
#include <windows.h>
#endif
#include <cmath>

namespace blast {
    VulkanSurface::VulkanSurface(VulkanContext* context, const GfxSurfaceDesc& desc)
    :GfxSurface(desc) {
        _context = context;
#if WIN32
        VkWin32SurfaceCreateInfoKHR sci = {};
        sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        sci.pNext = nullptr;
        sci.flags = 0;
        sci.hinstance = ::GetModuleHandle(nullptr);
        sci.hwnd = (HWND)desc.origin_surface;
        VK_ASSERT(vkCreateWin32SurfaceKHR(_context->GetInstance(), &sci, nullptr, &_surface));
#endif
        VulkanSwapchainSupportDetails swapChain_support = QuerySwapChainSupport(_surface);
        VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swapChain_support.formats);
        VkPresentModeKHR present_mode = ChooseSwapPresentMode(swapChain_support.present_modes);

        _present_mode = present_mode;
        _surface_format = surface_format;
    }

    VulkanSurface::~VulkanSurface() {
        vkDestroySurfaceKHR(_context->GetInstance(), _surface, nullptr);
    }

    VkSurfaceCapabilitiesKHR VulkanSurface::GetCapabilities() {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_context->GetPhyDevice(), _surface, &capabilities);
        return capabilities;
    }

    VulkanSwapchainSupportDetails VulkanSurface::QuerySwapChainSupport(VkSurfaceKHR surface) {
        VulkanSwapchainSupportDetails details;

        uint32_t num_formats;
        vkGetPhysicalDeviceSurfaceFormatsKHR(_context->GetPhyDevice(), surface, &num_formats, nullptr);

        if (num_formats != 0) {
            details.formats.resize(num_formats);
            vkGetPhysicalDeviceSurfaceFormatsKHR(_context->GetPhyDevice(), surface, &num_formats, details.formats.data());
        }

        uint32_t num_present_modes;
        vkGetPhysicalDeviceSurfacePresentModesKHR(_context->GetPhyDevice(), surface, &num_present_modes, nullptr);

        if (num_present_modes != 0) {
            details.present_modes.resize(num_present_modes);
            vkGetPhysicalDeviceSurfacePresentModesKHR(_context->GetPhyDevice(), surface, &num_present_modes, details.present_modes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR VulkanSurface::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {
        if (available_formats.size() == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED) {
            return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto& available_format : available_formats) {
            if (available_format.format == VK_FORMAT_B8G8R8A8_UNORM && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return available_format;
            }
        }

        return available_formats[0];
    }

    VkPresentModeKHR VulkanSurface::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes) {
        for (const auto& available_present_mode : available_present_modes) {
            if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return available_present_mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VulkanSwapchain::VulkanSwapchain(VulkanContext* context, const GfxSwapchainDesc& desc)
    :GfxSwapchain(desc) {
        _context = context;

        VulkanSurface* internel_surface = static_cast<VulkanSurface*>(desc.surface);

        VkBool32 support_present;
        VulkanQueue* graphics_queue = (VulkanQueue*)_context->GetQueue(QUEUE_TYPE_GRAPHICS);
        uint32_t queue_family_indice = graphics_queue->GetFamilyIndex();
        vkGetPhysicalDeviceSurfaceSupportKHR(_context->GetPhyDevice(), queue_family_indice, internel_surface->GetHandle(), &support_present);
        if (!support_present) {
            BLAST_LOGE("cannot find a graphics queue that also supports present operations.\n");
            return;
        }

        const auto caps = internel_surface->GetCapabilities();

        const uint32_t num_max_images = caps.maxImageCount;
        const uint32_t num_min_images = caps.minImageCount;
        uint32_t num_desired_images = num_min_images + 1;
        if (num_max_images != 0 && num_desired_images > num_max_images) {
            BLAST_LOGE("swap chain does not support %d images.\n", num_desired_images);
            num_desired_images = num_min_images;
        }
        _num_images = num_desired_images;

        VkExtent2D actualExtent;
        actualExtent.width = _width;
        actualExtent.height = _height;

        VkSwapchainCreateInfoKHR sci = {};
        sci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        sci.surface = internel_surface->GetHandle();
        sci.minImageCount = _num_images;
        sci.imageFormat = internel_surface->GetSurfaceFormat().format;
        sci.imageColorSpace = internel_surface->GetSurfaceFormat().colorSpace;
        sci.imageExtent = actualExtent;
        sci.imageArrayLayers = 1;
        sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        sci.queueFamilyIndexCount = 1;
        sci.pQueueFamilyIndices = &queue_family_indice;
        sci.preTransform = internel_surface->GetCapabilities().currentTransform;
        sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        sci.presentMode = internel_surface->GetPresentMode();
        sci.clipped = VK_TRUE;
        sci.oldSwapchain = VK_NULL_HANDLE;
        if (desc.old_swapchain) {
            VulkanSwapchain* old_swapchain = static_cast<VulkanSwapchain*>(desc.old_swapchain);
            sci.oldSwapchain = old_swapchain->_swapchain;
        }

        VK_ASSERT(vkCreateSwapchainKHR(_context->GetDevice(), &sci, nullptr, &_swapchain));

        std::vector<VkImage> images;
        vkGetSwapchainImagesKHR(_context->GetDevice(), _swapchain, &_num_images, nullptr);
        images.resize(_num_images);
        vkGetSwapchainImagesKHR(_context->GetDevice(), _swapchain, &_num_images, images.data());

        for (int i = 0; i < _num_images; i++) {
            GfxTextureDesc color_desc;
            color_desc.sample_count = SAMPLE_COUNT_1;
            color_desc.width = actualExtent.width;
            color_desc.height = actualExtent.height;
            color_desc.depth = 1;
            color_desc.num_mips = 1;
            color_desc.num_layers = 1;
            color_desc.format = ToGfxFormat(internel_surface->GetSurfaceFormat().format);
            color_desc.state = RESOURCE_STATE_UNDEFINED;
            color_desc.type = RESOURCE_TYPE_TEXTURE;
            color_desc.usage = RESOURCE_USAGE_GPU_ONLY;

            VulkanTexture* color_texture = new VulkanTexture(_context, color_desc, images[i]);
            _color_images.push_back(color_texture);

            GfxTextureDesc depth_stencil_desc;
            depth_stencil_desc.sample_count = SAMPLE_COUNT_1;
            depth_stencil_desc.width = actualExtent.width;
            depth_stencil_desc.height = actualExtent.height;
            depth_stencil_desc.depth = 1;
            depth_stencil_desc.num_mips = 1;
            depth_stencil_desc.num_layers = 1;
            depth_stencil_desc.format = FORMAT_D24_UNORM_S8_UINT;
            depth_stencil_desc.state = RESOURCE_STATE_UNDEFINED;
            depth_stencil_desc.type = RESOURCE_TYPE_TEXTURE;
            depth_stencil_desc.usage = RESOURCE_USAGE_GPU_ONLY;

            VulkanTexture* depth_stencil_texture = new VulkanTexture(_context, depth_stencil_desc);
            _depth_stencil_images.push_back(depth_stencil_texture);
        }
    }

    VulkanSwapchain::~VulkanSwapchain() {
        for (int i = 0; i < _color_images.size(); i++) {
            delete _color_images[i];
        }
        _color_images.clear();

        for (int i = 0; i < _depth_stencil_images.size(); i++) {
            delete _depth_stencil_images[i];
        }
        _depth_stencil_images.clear();
        vkDestroySwapchainKHR(_context->GetDevice(), _swapchain, nullptr);
    }

    GfxTexture* VulkanSwapchain::GetColorRenderTarget(uint32_t idx) {
        if (idx >= 0 && idx < _color_images.size()) {
            return _color_images[idx];
        }
        return nullptr;
    }

    GfxTexture* VulkanSwapchain::GetDepthRenderTarget(uint32_t idx) {
        if (idx >= 0 && idx < _depth_stencil_images.size()) {
            return _depth_stencil_images[idx];
        }
        return nullptr;
    }

    uint32_t VulkanSwapchain::GetImageCount() {
        return _num_images;
    }
}