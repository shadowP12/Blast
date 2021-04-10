#pragma once
#include "VulkanDefine.h"
#include "../GfxPipeline.h"
#include <map>
#include <vector>

namespace Blast {
    class VulkanContext;

    class VulkanRootSignature : public GfxRootSignature {
    public:
        VulkanRootSignature(VulkanContext* context, const GfxRootSignatureDesc& desc);
        ~VulkanRootSignature();
        void setSampler(const std::string& name, GfxSampler* sampler) override;
        void setTexture(const std::string& name, GfxTexture* texture) override;
        void setRWTexture(const std::string& name, GfxTexture* texture) override;
        void setUniformBuffer(const std::string& name, GfxBuffer* buffer, uint32_t size, uint32_t offset) override;
        void setRWBuffer(const std::string& name, GfxBuffer* buffer, uint32_t size, uint32_t offset) override;
        VkPipelineLayout getPipelineLayout() { return mPipelineLayout; }
        const std::vector<VkDescriptorSet>& getSets() { return mSets; };
    protected:
        VulkanContext* mContext = nullptr;
        std::vector<VkDescriptorSet> mSets;
        std::map<int, VkDescriptorSet> mSetMap;
        std::map<int, VkDescriptorSetLayout> mSetLayoutMap;
        VkPipelineLayout mPipelineLayout;
    };

    class VulkanGraphicsPipeline : public GfxGraphicsPipeline {
    public:
        VulkanGraphicsPipeline(VulkanContext* context, const GfxGraphicsPipelineDesc& desc);
        ~VulkanGraphicsPipeline();
        VkPipeline getHandle() { return mPipeline; }
    protected:
        VulkanContext* mContext = nullptr;
        VkPipeline mPipeline;
    };
}

