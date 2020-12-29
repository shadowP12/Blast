#include "VulkanPipeline.h"
#include "VulkanContext.h"
#include "VulkanRenderTarget.h"
#include "VulkanShader.h"

namespace Blast {
    VulkanRootSignature::VulkanRootSignature(VulkanContext *context, const GfxRootSignatureDesc &desc)
    :GfxRootSignature(desc) {
        mContext = context;

        std::map<int, std::vector<VkDescriptorSetLayoutBinding>> bindingsMap;
        if (SHADER_STAGE_VERT == (mStages & SHADER_STAGE_VERT)) {
            for (int i = 0; i < desc.vertex.resources.size(); ++i) {
                const GfxShaderResource& shaderResource = desc.vertex.resources[i];
                VkDescriptorSetLayoutBinding binding = {};
                binding.pImmutableSamplers = nullptr;
                binding.binding = shaderResource.reg;
                binding.descriptorCount = shaderResource.size;
                binding.descriptorType = toVulkanDescriptorType(shaderResource.type);
                binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                bindingsMap[shaderResource.set].push_back(binding);
            }
        }
        if (SHADER_STAGE_TESC == (mStages & SHADER_STAGE_TESC)) {
            for (int i = 0; i < desc.hull.resources.size(); ++i) {
                const GfxShaderResource& shaderResource = desc.hull.resources[i];
                VkDescriptorSetLayoutBinding binding = {};
                binding.pImmutableSamplers = nullptr;
                binding.binding = shaderResource.reg;
                binding.descriptorCount = shaderResource.size;
                binding.descriptorType = toVulkanDescriptorType(shaderResource.type);
                binding.stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                bindingsMap[shaderResource.set].push_back(binding);
            }
        }
        if (SHADER_STAGE_TESE == (mStages & SHADER_STAGE_TESE)) {
            for (int i = 0; i < desc.domain.resources.size(); ++i) {
                const GfxShaderResource& shaderResource = desc.domain.resources[i];
                VkDescriptorSetLayoutBinding binding = {};
                binding.pImmutableSamplers = nullptr;
                binding.binding = shaderResource.reg;
                binding.descriptorCount = shaderResource.size;
                binding.descriptorType = toVulkanDescriptorType(shaderResource.type);
                binding.stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                bindingsMap[shaderResource.set].push_back(binding);
            }
        }
        if (SHADER_STAGE_GEOM == (mStages & SHADER_STAGE_GEOM)) {
            for (int i = 0; i < desc.geometry.resources.size(); ++i) {
                const GfxShaderResource& shaderResource = desc.geometry.resources[i];
                VkDescriptorSetLayoutBinding binding = {};
                binding.pImmutableSamplers = nullptr;
                binding.binding = shaderResource.reg;
                binding.descriptorCount = shaderResource.size;
                binding.descriptorType = toVulkanDescriptorType(shaderResource.type);
                binding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;
                bindingsMap[shaderResource.set].push_back(binding);
            }
        }
        if (SHADER_STAGE_FRAG == (mStages & SHADER_STAGE_FRAG)) {
            for (int i = 0; i < desc.pixel.resources.size(); ++i) {
                const GfxShaderResource& shaderResource = desc.pixel.resources[i];
                VkDescriptorSetLayoutBinding binding = {};
                binding.pImmutableSamplers = nullptr;
                binding.binding = shaderResource.reg;
                binding.descriptorCount = shaderResource.size;
                binding.descriptorType = toVulkanDescriptorType(shaderResource.type);
                binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                bindingsMap[shaderResource.set].push_back(binding);
            }
        }

        std::vector<VkDescriptorSetLayout> layouts;
        for (auto& bindings : bindingsMap) {
            VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutCreateInfo.bindingCount = bindings.second.size();
            layoutCreateInfo.pBindings = bindings.second.data();
            VkDescriptorSetLayout layout = VK_NULL_HANDLE;
            VK_ASSERT(vkCreateDescriptorSetLayout(mContext->getDevice(), &layoutCreateInfo, nullptr, &layout));
            mSetLayoutMap[bindings.first] = layout;
            layouts.push_back(layout);

            VkDescriptorSetAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = mContext->getDescriptorPool();
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &layout;
            VkDescriptorSet set = VK_NULL_HANDLE;
            VK_ASSERT(vkAllocateDescriptorSets(mContext->getDevice(), &allocInfo, &set));
            mSetMap[bindings.first] = set;
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = layouts.size();
        pipelineLayoutInfo.pSetLayouts = layouts.data();

        VK_ASSERT(vkCreatePipelineLayout(mContext->getDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout));
    }

    VulkanRootSignature::~VulkanRootSignature() {
        for (auto& set : mSetMap) {
            vkFreeDescriptorSets(mContext->getDevice(), mContext->getDescriptorPool(), 1, &set.second);
        }

        for (auto& layout : mSetLayoutMap) {
            vkDestroyDescriptorSetLayout(mContext->getDevice(), layout.second, nullptr);
        }

        vkDestroyPipelineLayout(mContext->getDevice(), mPipelineLayout, nullptr);
    }
}