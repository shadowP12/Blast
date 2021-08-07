#include "VulkanShader.h"
#include "VulkanContext.h"

namespace blast {
    VulkanShader::VulkanShader(VulkanContext *context, const GfxShaderDesc &desc)
    :GfxShader(desc) {
        _context = context;
        VkShaderModuleCreateInfo smci{};
        smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        smci.pNext = nullptr;
        smci.flags = 0;
        smci.codeSize = desc.bytes.size() * sizeof(uint32_t);
        smci.pCode = desc.bytes.data();

        VK_ASSERT(vkCreateShaderModule(_context->GetDevice(), &smci, nullptr, &_shader));
    }

    VulkanShader::~VulkanShader() {
        vkDestroyShaderModule(_context->GetDevice(), _shader, nullptr);
    }
}