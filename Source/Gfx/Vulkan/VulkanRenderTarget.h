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
        VkFramebuffer getFramebuffer() { return mFramebuffer; }
        uint32_t getWidth() { return mWidth;}
        uint32_t getHeight() { return mHeight; }
    protected:
        VulkanContext* mContext = nullptr;
        VkRenderPass mRenderPass;
        VkFramebuffer mFramebuffer;
        uint32_t mWidth;
        uint32_t mHeight;
    };
}