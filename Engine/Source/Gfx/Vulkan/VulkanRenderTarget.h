#pragma once
#include "VulkanDefine.h"
#include "../GfxRenderTarget.h"

namespace Blast {
    class VulkanDevice;

    class VulkanRenderTarget : public GfxRenderTarget {
    public:
        VulkanRenderTarget(VulkanDevice *device, const GfxRenderTargetDesc &desc);
        virtual ~VulkanRenderTarget();
    protected:
        VulkanDevice* mDevice = nullptr;
        VkRenderPass mRenderPass;
        VkFramebuffer mFramebuffer;
    };
}