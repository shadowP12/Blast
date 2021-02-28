#pragma once
#include "VulkanDefine.h"
#include "../GfxCommandBuffer.h"

namespace Blast {
    class VulkanContext;
    class VulkanCommandBufferPool : public GfxCommandBufferPool {
    public:
        VulkanCommandBufferPool(VulkanContext* context, const GfxCommandBufferPoolDesc& desc);
        ~VulkanCommandBufferPool();
        GfxCommandBuffer* allocBuf(bool secondary) override;
        QueueType getQueueType() { return mQueueType; }
        VkCommandPool getHandle() { return mPool; }
    protected:
        VulkanContext* mContext = nullptr;
        VkCommandPool mPool;
        QueueType mQueueType;
    };

    class VulkanCommandBuffer : public GfxCommandBuffer {
    public:
        VulkanCommandBuffer(VulkanContext* context, const GfxCommandBufferDesc& desc);
        ~VulkanCommandBuffer();
        VkCommandBuffer getHandle() { return mCommandBuffer; }
        void begin() override;
        void end() override;
        void bindRenderPass(GfxRenderPass* renderPass, const GfxClearValue& clearValue) override;
        void unbindRenderPass() override;
        void setViewport(float x, float y, float w, float h) override;
        void setScissor(int x, int y, int w, int h) override;
        void bindVertexBuffer(GfxBuffer* vertexBuffer, uint32_t offset) override;
        void bindIndexBuffer(GfxBuffer* indexBuffer, uint32_t offset, IndexType type) override;
        void bindGraphicsPipeline(GfxGraphicsPipeline* pipeline) override;
        void bindRootSignature(GfxRootSignature* rootSignature, PipelineType type) override;
        void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
        void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) override;
        void copyToBuffer(GfxBuffer* srcBuffer, uint32_t srcOffset, GfxBuffer* dstBuffer, uint32_t dstOffset, uint32_t size) override;
        void copyToImage(GfxBuffer* srcBuffer, GfxTexture* dstTexture, const GfxCopyToImageHelper& helper) override;
        void setBarrier(uint32_t inBufferBarrierCount, GfxBufferBarrier* inBufferBarriers,
                        uint32_t inTextureBarrierCount, GfxTextureBarrier* inTextureBarriers) override;
    protected:
        VulkanContext* mContext = nullptr;
        VulkanCommandBufferPool* mPool;
        VkCommandBuffer mCommandBuffer;
    };
}