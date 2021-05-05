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
        void setSampler(const uint8_t& set, const uint8_t& reg, GfxSampler* sampler) override;
        void setTexture(const uint8_t& set, const uint8_t& reg, GfxTexture* texture) override;
        void setCombinedSampler(const uint8_t& set, const uint8_t& reg, GfxTexture* texture, GfxSampler* sampler) override;
        void setRWTexture(const uint8_t& set, const uint8_t& reg, GfxTexture* texture) override;
        void setUniformBuffer(const uint8_t& set, const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) override;
        void setRWBuffer(const uint8_t& set, const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) override;
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

