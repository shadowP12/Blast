#include "VulkanFramebuffer.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#include <vector>

namespace blast {
    VulkanFramebuffer::VulkanFramebuffer(VulkanContext* context, const GfxFramebufferDesc& desc)
    :GfxFramebuffer(desc) {
        _context = context;

        // VK_ATTACHMENT_LOAD_OP_LOAD = 加载Attachment时保留之前所有内容
        // VK_ATTACHMENT_LOAD_OP_CLEAR : 加载Attachment时清空所有内容
        // VK_ATTACHMENT_LOAD_OP_DONT_CARE : 加载Attachment时由驱动去决定加载的内容，适合对整个area进行overwrite时使用
        // VK_ATTACHMENT_STORE_OP_STORE : 存储Attachment时保留之前写入内容
        // VK_ATTACHMENT_STORE_OP_DONT_CARE : 存储Attachment时由驱动去决定存储的内容
        std::vector<VkAttachmentDescription> attachments;
        std::vector<VkAttachmentReference> color_attachment_refs;
        VkAttachmentReference depth_stencil_attachment_ref;

        uint32_t attachment_idx = 0;
        for (uint32_t i = 0; i < desc.num_colors; i++) {
            VulkanTextureView* internel_view = (VulkanTextureView*)desc.colors[i];
            
            VkAttachmentDescription attachment_desc = {};
            attachment_desc.flags = 0;
            attachment_desc.format = ToVulkanFormat(internel_view->GetTexture()->GetFormat());
            attachment_desc.samples = ToVulkanSampleCount(desc.sample_count);
            attachment_desc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment_desc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachment_desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments.push_back(attachment_desc);

            VkAttachmentReference colorReference;
            colorReference.attachment = attachment_idx;
            colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            color_attachment_refs.push_back(colorReference);

            attachment_idx++;
        }
        
        if (desc.has_depth_stencil) {
            VulkanTextureView* internel_view = (VulkanTextureView*)desc.depth_stencil;
            
            VkAttachmentDescription attachment_desc = {};
            attachment_desc.flags = 0;
            attachment_desc.format = ToVulkanFormat(internel_view->GetTexture()->GetFormat());
            attachment_desc.samples = ToVulkanSampleCount(desc.sample_count);
            attachment_desc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment_desc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachment_desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            attachments.push_back(attachment_desc);

            depth_stencil_attachment_ref.attachment = attachment_idx;
            depth_stencil_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            attachment_idx++;
        }

        VkSubpassDescription subpass = {};
        subpass.flags = 0;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.inputAttachmentCount = 0;
        subpass.pInputAttachments = NULL;
        subpass.colorAttachmentCount = desc.num_colors;
        subpass.pResolveAttachments = NULL;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments = NULL;

        if (desc.num_colors > 0) {
            subpass.pColorAttachments = color_attachment_refs.data();
        } else {
            subpass.pColorAttachments = nullptr;
        }

        if (desc.has_depth_stencil) {
            subpass.pDepthStencilAttachment = &depth_stencil_attachment_ref;
        } else {
            subpass.pDepthStencilAttachment = nullptr;
        }

        VkRenderPassCreateInfo rpci;
        rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpci.pNext = NULL;
        rpci.flags = 0;
        rpci.attachmentCount = attachment_idx;
        rpci.pAttachments = attachments.data();
        rpci.subpassCount = 1;
        rpci.pSubpasses = &subpass;
        rpci.dependencyCount = 0;
        rpci.pDependencies = NULL;

        VK_ASSERT(vkCreateRenderPass(_context->GetDevice(), &rpci, nullptr, &_render_pass));

        std::vector<VkImageView> attachment_views;
        for (int i = 0; i < desc.num_colors; i++) {
            VulkanTextureView* internel_view = (VulkanTextureView*)desc.colors[i];
            attachment_views.push_back(internel_view->GetHandle());
        }

        if(desc.has_depth_stencil) {
            VulkanTextureView* internel_view = (VulkanTextureView*)desc.depth_stencil;
            attachment_views.push_back(internel_view->GetHandle());
        }

        VkFramebufferCreateInfo fci;
        fci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fci.pNext = nullptr;
        fci.flags = 0;
        fci.attachmentCount = attachment_views.size();
        fci.pAttachments = attachment_views.data();
        fci.width = _width;
        fci.height = _height;
        fci.layers = 1;
        fci.renderPass = _render_pass;

        VK_ASSERT(vkCreateFramebuffer(_context->GetDevice(), &fci, nullptr, &_framebuffer));
    }

    VulkanFramebuffer::~VulkanFramebuffer() {
        vkDestroyRenderPass(_context->GetDevice(), _render_pass, nullptr);
        vkDestroyFramebuffer(_context->GetDevice(), _framebuffer, nullptr);
    }
}