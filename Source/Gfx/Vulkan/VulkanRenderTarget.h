#pragma once
#include "VulkanDefine.h"
#include "../GfxRenderTarget.h"

namespace Blast {
    class VulkanContext;

    class VulkanRenderTarget : public GfxRenderTarget {
    public:
        VulkanRenderTarget(VulkanContext *device, const GfxRenderTargetDesc &desc);
        virtual ~VulkanRenderTarget();
        VkRenderPass getRenderPass() { return mRenderPass; }
    protected:
        VulkanContext* mContext = nullptr;
        VkRenderPass mRenderPass;
        VkFramebuffer mFramebuffer;
    };
}