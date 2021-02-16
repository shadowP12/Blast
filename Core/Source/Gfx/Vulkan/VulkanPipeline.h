#pragma once
#include "VulkanDefine.h"
#include "../GfxPipeline.h"
#include <map>

namespace Blast {
    class VulkanContext;

    class VulkanRootSignature : public GfxRootSignature {
    public:
        VulkanRootSignature(VulkanContext* context, const GfxRootSignatureDesc& desc);
        virtual ~VulkanRootSignature();
        VkPipelineLayout getPipelineLayout() { return mPipelineLayout; }
    protected:
        VulkanContext* mContext = nullptr;
        std::map<int, VkDescriptorSet> mSetMap;
        std::map<int, VkDescriptorSetLayout> mSetLayoutMap;
        VkPipelineLayout mPipelineLayout;
    };

    class VulkanGraphicsPipeline : public GfxGraphicsPipeline {
    public:
        VulkanGraphicsPipeline(VulkanContext* context, const GfxGraphicsPipelineDesc& desc);
        virtual ~VulkanGraphicsPipeline();
        VkPipelineLayout getHandle() { return mPipeline; }
    protected:
        VulkanContext* mContext = nullptr;
        VkPipeline mPipeline;
    };
}

