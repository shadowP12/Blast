#pragma once
#include "VulkanDefine.h"
#include "../GfxShader.h"

namespace Blast {
    class VulkanContext;

    class VulkanShader : public GfxShader {
    public:
        VulkanShader(VulkanContext* context, const GfxShaderDesc& desc);
        ~VulkanShader();
        VkShaderModule getHandle() { return mShader; }
    protected:
        VulkanContext* mContext = nullptr;
        VkShaderModule mShader;
    };
}