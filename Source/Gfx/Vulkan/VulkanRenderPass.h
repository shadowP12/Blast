#pragma once
#include "VulkanDefine.h"
#include "../GfxRenderPass.h"

namespace Blast {
    class VulkanContext;

    class VulkanRenderPass : public GfxRenderPass {
    public:
        VulkanRenderPass(VulkanContext *device, const GfxRenderPassDesc &desc);
        virtual ~VulkanRenderPass();
        VkRenderPass getRenderPass() { return mRenderPass; }
        VkFramebuffer getFramebuffer() { return mFramebuffer; }
        uint32_t getWidth() { return mWidth;}
        uint32_t getHeight() { return mHeight; }
        GfxTexture* getColorRT(int idx) override;
        GfxTexture* getDepthRT() override;
    protected:
        VulkanContext* mContext = nullptr;
        VkRenderPass mRenderPass;
        VkFramebuffer mFramebuffer;
        uint32_t mWidth;
        uint32_t mHeight;
    };
}