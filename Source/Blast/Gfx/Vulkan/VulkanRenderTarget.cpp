#include "VulkanRenderTarget.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#include <vector>

namespace Blast {
    VulkanRenderPass::VulkanRenderPass(VulkanContext *context, const GfxRenderPassDesc &desc)
    :GfxRenderPass(desc) {
        mContext = context;

        /*
         * note: VK_ATTACHMENT_LOAD_OP_DONT_CARE : 交由驱动去决定加载内容,适合对整个area进行overwrite时使用
         */
        std::vector<VkAttachmentDescription> attachments;
        std::vector<VkAttachmentReference> colorAttachmentRefs;
        VkAttachmentReference depthStencilAttachmentRef;

        uint32_t attachmentIdx = 0;
        for (uint32_t i = 0; i < mNumColorAttachments; i++) {
            VkAttachmentDescription attachmentDesc = {};
            attachmentDesc.flags = 0;
            attachmentDesc.format = toVulkanFormat(mColors[i].format);
            attachmentDesc.samples = toVulkanSampleCount(mColors[i].sampleCount);
            attachmentDesc.loadOp = toVulkanLoadOp(mColors[i].loadOp);
            attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments.push_back(attachmentDesc);

            VkAttachmentReference colorReference;
            colorReference.attachment = attachmentIdx;
            colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentRefs.push_back(colorReference);

            attachmentIdx++;
        }
        if (mHasDepthStencil) {
            VkAttachmentDescription attachmentDesc = {};
            attachmentDesc.flags = 0;
            attachmentDesc.format = toVulkanFormat(mDepthStencil.format);
            attachmentDesc.samples = toVulkanSampleCount(mDepthStencil.sampleCount);
            attachmentDesc.loadOp = toVulkanLoadOp(mDepthStencil.depthLoadOp);
            attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDesc.stencilLoadOp = toVulkanLoadOp(mDepthStencil.stencilLoadOp);
            attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachments.push_back(attachmentDesc);

            depthStencilAttachmentRef.attachment = attachmentIdx;
            depthStencilAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            attachmentIdx++;
        }

        VkSubpassDescription subpass = {};
        subpass.flags = 0;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = NULL;
        subpass.colorAttachmentCount = mNumColorAttachments;
        subpass.pResolveAttachments = NULL;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = NULL;

        if (mNumColorAttachments > 0) {
            subpass.pColorAttachments = colorAttachmentRefs.data();
        } else {
            subpass.pColorAttachments = nullptr;
        }

        if (mHasDepthStencil) {
            subpass.pDepthStencilAttachment = &depthStencilAttachmentRef;
        } else {
            subpass.pDepthStencilAttachment = nullptr;
        }

        VkRenderPassCreateInfo renderPassInfo;
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = NULL;
        renderPassInfo.flags = 0;
        renderPassInfo.attachmentCount = attachmentIdx;
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 0;
        renderPassInfo.pDependencies = NULL;

        VK_ASSERT(vkCreateRenderPass(mContext->getDevice(), &renderPassInfo, nullptr, &(mRenderPass)));
    }

    VulkanRenderPass::~VulkanRenderPass() {
        vkDestroyRenderPass(mContext->getDevice(), mRenderPass, nullptr);
    }

    VulkanFramebuffer::VulkanFramebuffer(VulkanContext* context, const GfxFramebufferDesc& desc)
    :GfxFramebuffer(desc) {
        mContext = context;

        VulkanRenderPass* internelRenderPass = static_cast<VulkanRenderPass*>(desc.renderPass);

        std::vector<VkImageView> attachmentViews;
        for (int i = 0; i < desc.numColorAttachments; i++) {
            VulkanTexture* vulkanTexture = (VulkanTexture*)desc.colors[i].target;
            attachmentViews.push_back(vulkanTexture->getSRV(desc.colors[i].layer, desc.colors[i].level));
        }

        if(desc.hasDepthStencil) {
            VulkanTexture* vulkanTexture = (VulkanTexture*)desc.depthStencil.target;
            attachmentViews.push_back(vulkanTexture->getSRV(desc.depthStencil.layer, desc.depthStencil.level));
        }

        VkFramebufferCreateInfo framebufferInfo;
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.pNext = nullptr;
        framebufferInfo.flags = 0;
        framebufferInfo.attachmentCount = attachmentViews.size();
        framebufferInfo.pAttachments = attachmentViews.data();
        framebufferInfo.width = mWidth;
        framebufferInfo.height = mHeight;
        framebufferInfo.layers = 1;
        framebufferInfo.renderPass = internelRenderPass->getHandle();

        VK_ASSERT(vkCreateFramebuffer(mContext->getDevice(), &framebufferInfo, nullptr, &mFramebuffer));
    }

    VulkanFramebuffer::~VulkanFramebuffer() {
        vkDestroyFramebuffer(mContext->getDevice(), mFramebuffer, nullptr);
    }
}