#pragma once
#include "VulkanDefine.h"
#include "../GfxSampler.h"

namespace Blast {
    class VulkanContext;

    class VulkanSampler : public GfxSampler {
    public:
        VulkanSampler(VulkanContext* context, const GfxSamplerDesc& desc);
        ~VulkanSampler();
        VkSampler getHandle() { return mSampler; }
    protected:
        VulkanContext* mContext = nullptr;
        VkSampler mSampler;
    };
}