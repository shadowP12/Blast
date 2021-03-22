#pragma once
#include "VulkanDefine.h"
#include "../GfxRenderTarget.h"

namespace Blast {
    class VulkanContext;

    class VulkanRenderPass : public GfxRenderPass {
    public:
        VulkanRenderPass(VulkanContext* context, const GfxRenderPassDesc &desc);
        ~VulkanRenderPass();
        VkRenderPass getHandle() { return mRenderPass; }
    protected:
        VulkanContext* mContext = nullptr;
        VkRenderPass mRenderPass;
    };

    class VulkanFramebuffer : public GfxFramebuffer {
    public:
        VulkanFramebuffer(VulkanContext* context, const GfxFramebufferDesc &desc);
        ~VulkanFramebuffer();
        VkFramebuffer getHandle() { return mFramebuffer; }
    protected:
        VulkanContext* mContext = nullptr;
        VkFramebuffer mFramebuffer;
    };
}