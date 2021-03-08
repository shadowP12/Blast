#include "VulkanSampler.h"
#include "../GfxContext.h"
#include "VulkanContext.h"

namespace Blast {
    VulkanSampler::VulkanSampler(VulkanContext* context, const GfxSamplerDesc& desc)
    : GfxSampler(desc) {
        mContext = context;

        // todo: 扩展sampler功能
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = toVulkanFilter(mMagFilter);
        samplerInfo.minFilter = toVulkanFilter(mMinFilter);
        samplerInfo.mipmapMode = toVulkanMipmapMode(mMipmapMode);
        samplerInfo.addressModeU = toVulkanAddressMode(mAddressU);
        samplerInfo.addressModeV = toVulkanAddressMode(mAddressV);
        samplerInfo.addressModeW = toVulkanAddressMode(mAddressW);
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 0.0f;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        VK_ASSERT(vkCreateSampler(mContext->getDevice(), &samplerInfo, nullptr, &mSampler));
    }

    VulkanSampler::~VulkanSampler() {
        vkDestroySampler(mContext->getDevice(), mSampler, nullptr);
    }
}