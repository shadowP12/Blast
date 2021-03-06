#pragma once
#include "VulkanDefine.h"
#include "../GfxRenderPass.h"

namespace Blast {
    class VulkanContext;

    class VulkanRenderPass : public GfxRenderPass {
    public:
        VulkanRenderPass(VulkanContext* context, const GfxRenderPassDesc &desc);
        ~VulkanRenderPass();
        VkRenderPass getHandle() { return mRenderPass; }
        VkFramebuffer getFramebuffer() { return mFramebuffer; }
    protected:
        VulkanContext* mContext = nullptr;
        VkRenderPass mRenderPass;
        VkFramebuffer mFramebuffer;
    };
}