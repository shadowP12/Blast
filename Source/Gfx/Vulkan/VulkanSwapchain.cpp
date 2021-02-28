#include "VulkanSwapchain.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
#if WIN32
#include <windows.h>
#endif

namespace Blast {
    VulkanSwapchain::VulkanSwapchain(VulkanContext* context, const GfxSwapchainDesc& desc)
    :GfxSwapchain(desc){
        mContext = context;
#if WIN32
        VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
        surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext = nullptr;
        surfaceInfo.flags = 0;
        surfaceInfo.hinstance = ::GetModuleHandle(nullptr);
        surfaceInfo.hwnd = (HWND)desc.windowHandle;
        VK_ASSERT(vkCreateWin32SurfaceKHR(mContext->getInstance(), &surfaceInfo, nullptr, &mSurface));
#endif

        VkBool32 supportsPresent;
        VulkanQueue* GraphicsQueue = (VulkanQueue*)mContext->getQueue(QUEUE_TYPE_GRAPHICS);
        vkGetPhysicalDeviceSurfaceSupportKHR(mContext->getPhyDevice(), GraphicsQueue->getFamilyIndex(), mSurface, &supportsPresent);
        if (!supportsPresent) {
            BLAST_LOGE("cannot find a graphics queue that also supports present operations.\n");
            return;
        }

        VkExtent2D swapChainExtent;
        swapChainExtent.width = mWidth;
        swapChainExtent.height = mHeight;

        VulkanSwapchainSupportDetails swapChainSupport = querySwapChainSupport();
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        mImageCount = imageCount;

        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = mSurface;
        swapchainInfo.minImageCount = imageCount;
        swapchainInfo.imageFormat = surfaceFormat.format;
        swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainInfo.imageExtent = swapChainExtent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.queueFamilyIndexCount = 0;
        swapchainInfo.pQueueFamilyIndices = nullptr;
        swapchainInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode = presentMode;
        swapchainInfo.clipped = VK_TRUE;

        VK_ASSERT(vkCreateSwapchainKHR(mContext->getDevice(), &swapchainInfo, nullptr, &mSwapchain));

        std::vector<VkImage> images;
        Format imageFormat = toGfxFormat(surfaceFormat.format);
        vkGetSwapchainImagesKHR(mContext->getDevice(), mSwapchain, &imageCount, nullptr);
        images.resize(imageCount);
        vkGetSwapchainImagesKHR(mContext->getDevice(), mSwapchain, &imageCount, images.data());

        for (int i = 0; i < imageCount; i++) {
            GfxTextureDesc colorDesc;
            colorDesc.sampleCount = SAMPLE_COUNT_1;
            colorDesc.width = mWidth;
            colorDesc.height = mHeight;
            colorDesc.depth = 1;
            colorDesc.mipLevels = 1;
            colorDesc.arrayLayers = 1;
            colorDesc.format = imageFormat;
            colorDesc.state = RESOURCE_STATE_UNDEFINED;
            colorDesc.type = RESOURCE_TYPE_TEXTURE;
            colorDesc.usage = RESOURCE_USAGE_GPU_ONLY;
            colorDesc.colorAtt = true;

            VulkanTexture* colorTexture = new VulkanTexture(mContext, images[i], colorDesc);
            mColorImages.push_back(colorTexture);

            GfxTextureDesc depthStencilDesc;
            depthStencilDesc.sampleCount = SAMPLE_COUNT_1;
            depthStencilDesc.width = mWidth;
            depthStencilDesc.height = mHeight;
            depthStencilDesc.depth = 1;
            depthStencilDesc.mipLevels = 1;
            depthStencilDesc.arrayLayers = 1;
            depthStencilDesc.format = FORMAT_D24_UNORM_S8_UINT;
            depthStencilDesc.state = RESOURCE_STATE_UNDEFINED;
            depthStencilDesc.type = RESOURCE_TYPE_TEXTURE;
            depthStencilDesc.usage = RESOURCE_USAGE_GPU_ONLY;
            depthStencilDesc.depthStencilAtt = true;

            VulkanTexture* depthStencilTexture = new VulkanTexture(mContext, depthStencilDesc);
            mDepthStencilImages.push_back(depthStencilTexture);
        }
    }

    VulkanSwapchain::~VulkanSwapchain() {
        for (int i = 0; i < mColorImages.size(); i++) {
            delete mColorImages[i];
        }
        mColorImages.clear();

        for (int i = 0; i < mDepthStencilImages.size(); i++) {
            delete mDepthStencilImages[i];
        }
        mDepthStencilImages.clear();

        vkDestroySwapchainKHR(mContext->getDevice(), mSwapchain, nullptr);
        vkDestroySurfaceKHR(mContext->getInstance(), mSurface, nullptr);
    }

    GfxTexture* VulkanSwapchain::getColorRenderTarget(uint32_t idx) {
        if (idx >= 0 && idx < mColorImages.size()) {
            return mColorImages[idx];
        }
        return nullptr;
    }

    GfxTexture* VulkanSwapchain::getDepthRenderTarget(uint32_t idx) {
        if (idx >= 0 && idx < mDepthStencilImages.size()) {
            return mDepthStencilImages[idx];
        }
        return nullptr;
    }

    VulkanSwapchainSupportDetails VulkanSwapchain::querySwapChainSupport() {
        VulkanSwapchainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mContext->getPhyDevice(), mSurface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->getPhyDevice(), mSurface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(mContext->getPhyDevice(), mSurface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->getPhyDevice(), mSurface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(mContext->getPhyDevice(), mSurface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
            return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    uint32_t VulkanSwapchain::getImageCount() {
        return mImageCount;
    }
}