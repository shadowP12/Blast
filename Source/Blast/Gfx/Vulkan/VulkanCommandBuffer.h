#pragma once
#include "VulkanDefine.h"
#include "../GfxCommandBuffer.h"

namespace blast {
    class VulkanContext;
    class VulkanRootSignature;
    class VulkanCommandBufferPool : public GfxCommandBufferPool {
    public:
        VulkanCommandBufferPool(VulkanContext* context, const GfxCommandBufferPoolDesc& desc);

        ~VulkanCommandBufferPool();

        GfxCommandBuffer* AllocBuffer(bool secondary) override;

        void DeleteBuffer(GfxCommandBuffer* buffer) override;

        QueueType GetQueueType() { return _queue_type; }

        VkCommandPool GetHandle() { return _pool; }

    protected:
        VulkanContext* _context = nullptr;
        VkCommandPool _pool;
        QueueType _queue_type;
    };

    class VulkanCommandBuffer : public GfxCommandBuffer {
    public:
        VulkanCommandBuffer(VulkanContext* context, const GfxCommandBufferDesc& desc);

        ~VulkanCommandBuffer();

        VkCommandBuffer GetHandle() { return _command_buffer; }

        void Begin() override;

        void End() override;

        void BindFramebuffer(GfxFramebuffer* framebuffer) override;

        void UnbindFramebuffer() override;

        void ClearFramebuffer(GfxFramebuffer* framebuffer, const GfxClearValue& clear_value) override;

        void SetViewport(float x, float y, float w, float h) override;

        void SetScissor(int x, int y, int w, int h) override;

        void BindVertexBuffer(GfxBuffer* vertex_buffer, uint32_t offset) override;

        void BindIndexBuffer(GfxBuffer* index_buffer, uint32_t offset, IndexType type) override;

        void BindGraphicsPipeline(GfxGraphicsPipeline* pipeline) override;

        void BindDescriptorSets(GfxRootSignature* root_signature, uint32_t num_sets, GfxDescriptorSet** sets) override;

        void Draw(uint32_t num_vertices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_instance) override;

        void DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_index, uint32_t vertex_offset, uint32_t first_instance) override;

        void CopyToBuffer(const GfxCopyToBufferRange& range) override;

        void CopyToImage(const GfxCopyToImageRange& range) override;

        void SetBarrier(uint32_t num_buffer_barriers, GfxBufferBarrier* buffer_barriers,
                                uint32_t num_texture_barriers, GfxTextureBarrier* texture_barriers) override;
    protected:
        VulkanContext* _context = nullptr;
        VulkanCommandBufferPool* _pool;
        VkCommandBuffer _command_buffer;
    };
}