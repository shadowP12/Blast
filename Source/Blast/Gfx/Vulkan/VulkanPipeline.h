#pragma once
#include "VulkanDefine.h"
#include "../GfxPipeline.h"
#include <map>
#include <vector>

namespace blast {
    class VulkanContext;
    class VulkanRootSignature;

    class VulkanDescriptorSet : public GfxDescriptorSet {
    public:
        VulkanDescriptorSet(VulkanContext* context, VulkanRootSignature* root_signature, const uint8_t& set);

        ~VulkanDescriptorSet();

        VkDescriptorSet GetHandle() { return _set; }

        void SetSampler(const uint8_t& reg, GfxSampler* sampler) override;

        void SetTexture(const uint8_t& reg, GfxTextureView* texture_view) override;

        void SetStorgeTexture(const uint8_t& reg, GfxTextureView* texture_view) override;

        void SetCombinedSampler(const uint8_t& reg, GfxTextureView* texture_view, GfxSampler* sampler) override;

        void SetUniformBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) override;

        void SetStorgeBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) override;

    protected:
        VulkanContext* _context = nullptr;
        VulkanRootSignature* _root_signature = nullptr;
        VkDescriptorSet _set = VK_NULL_HANDLE;
    };

    class VulkanRootSignature : public GfxRootSignature {
    public:
        VulkanRootSignature(VulkanContext* context, const GfxRootSignatureDesc& desc);

        ~VulkanRootSignature();

        GfxDescriptorSet* AllocateSet(const uint8_t& set) override;

        void DeleteSet(GfxDescriptorSet* set) override;

        VkPipelineLayout GetPipelineLayout() { return _pipeline_layout; }

    protected:
        friend class VulkanDescriptorSet;
        VulkanContext* _context = nullptr;
        std::map<uint32_t, VkDescriptorSetLayout> _set_layout_map;
        VkPipelineLayout _pipeline_layout;
    };

    class VulkanGraphicsPipeline : public GfxGraphicsPipeline {
    public:
        VulkanGraphicsPipeline(VulkanContext* context, const GfxGraphicsPipelineDesc& desc);

        ~VulkanGraphicsPipeline();

        VkPipeline GetHandle() { return _pipeline; }

    protected:
        VulkanContext* _context = nullptr;
        VkPipeline _pipeline;
    };
}

