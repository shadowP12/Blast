#pragma once
#include "VulkanDefine.h"
#include "../GfxSampler.h"

namespace blast {
    class VulkanContext;

    class VulkanSampler : public GfxSampler {
    public:
        VulkanSampler(VulkanContext* context, const GfxSamplerDesc& desc);

        ~VulkanSampler();

        VkSampler GetHandle() { return _sampler; }

    protected:
        VulkanContext* _context = nullptr;
        VkSampler _sampler;
    };
}