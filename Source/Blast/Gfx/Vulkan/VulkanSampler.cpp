#include "VulkanSampler.h"
#include "../GfxContext.h"
#include "VulkanContext.h"

namespace blast {
    VulkanSampler::VulkanSampler(VulkanContext* context, const GfxSamplerDesc& desc)
    : GfxSampler(desc) {
        _context = context;

        // todo: 扩展sampler功能
        VkSamplerCreateInfo sci = {};
        sci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sci.magFilter = ToVulkanFilter(_mag_filter);
        sci.minFilter = ToVulkanFilter(_min_filter);
        sci.mipmapMode = ToVulkanMipmapMode(_mipmap_mode);
        sci.addressModeU = ToVulkanAddressMode(_address_u);
        sci.addressModeV = ToVulkanAddressMode(_address_v);
        sci.addressModeW = ToVulkanAddressMode(_address_w);
        sci.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sci.anisotropyEnable = VK_FALSE;
        sci.maxAnisotropy = 0.0f;
        sci.unnormalizedCoordinates = VK_FALSE;
        sci.compareEnable = VK_FALSE;
        sci.compareOp = VK_COMPARE_OP_ALWAYS;

        VK_ASSERT(vkCreateSampler(_context->GetDevice(), &sci, nullptr, &_sampler));
    }

    VulkanSampler::~VulkanSampler() {
        vkDestroySampler(_context->GetDevice(), _sampler, nullptr);
    }
}