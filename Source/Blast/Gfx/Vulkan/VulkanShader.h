#pragma once
#include "VulkanDefine.h"
#include "../GfxShader.h"

namespace blast {
    class VulkanContext;

    class VulkanShader : public GfxShader {
    public:
        VulkanShader(VulkanContext* context, const GfxShaderDesc& desc);

        ~VulkanShader();

        VkShaderModule GetHandle() { return _shader; }

    protected:
        VulkanContext* _context = nullptr;
        VkShaderModule _shader;
    };
}