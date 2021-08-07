#pragma once
#include "VulkanDefine.h"
#include "../GfxFramebuffer.h"

namespace blast {
    class VulkanContext;

    class VulkanFramebuffer : public GfxFramebuffer {
    public:
        VulkanFramebuffer(VulkanContext* context, const GfxFramebufferDesc &desc);

        ~VulkanFramebuffer();

        VkFramebuffer GetHandle() { return _framebuffer; }

        VkRenderPass GetRenderPass() { return _render_pass; }
    protected:
        VulkanContext* _context = nullptr;
        VkFramebuffer _framebuffer;
        VkRenderPass _render_pass;
    };
}