#include "VulkanShader.h"
#include "VulkanContext.h"

namespace Blast {
    VulkanShader::VulkanShader(VulkanContext *context, const GfxShaderDesc &desc)
    :GfxShader(desc) {
        mContext = context;
        VkShaderModuleCreateInfo shaderInfo;
        shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderInfo.pNext = nullptr;
        shaderInfo.flags = 0;
        shaderInfo.codeSize = desc.bytes.size() * sizeof(uint32_t);
        shaderInfo.pCode = desc.bytes.data();

        VK_ASSERT(vkCreateShaderModule(mContext->getDevice(), &shaderInfo, nullptr, &mShader));
    }

    VulkanShader::~VulkanShader() {
        vkDestroyShaderModule(mContext->getDevice(), mShader, nullptr);
    }
}