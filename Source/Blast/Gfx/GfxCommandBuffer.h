#pragma once
#include "GfxDefine.h"

namespace Blast {
    class GfxBuffer;
    class GfxTexture;
    class GfxQueue;
    class GfxCommandBuffer;
    class GfxRenderPass;
    class GfxFramebuffer;
    class GfxDescriptorSet;
    class GfxRootSignature;
    class GfxGraphicsPipeline;

    struct GfxCommandBufferPoolDesc {
        GfxQueue* queue = nullptr;
        bool transient = false;
    };

    class GfxCommandBufferPool {
    public:
        GfxCommandBufferPool(const GfxCommandBufferPoolDesc& desc);
        virtual GfxCommandBuffer* allocBuf(bool secondary) = 0;
    protected:
    };

    struct GfxCommandBufferDesc {
        GfxCommandBufferPool* pool = nullptr;
        bool secondary = false;
    };

    struct GfxClearValue {
        float color[4];
        float depth;
        uint32_t stencil;
    };

    struct GfxCopyToImageHelper {
        uint32_t bufferOffset = 0;
        uint32_t level = 0;
        uint32_t layer = 0;
    };

    struct GfxBufferBarrier {
        GfxBuffer* buffer;
        ResourceState newState;
    };

    struct GfxTextureBarrier {
        GfxTexture* texture;
        ResourceState newState;
    };

    class GfxCommandBuffer {
    public:
        GfxCommandBuffer(const GfxCommandBufferDesc& desc);
        virtual void begin() = 0;
        virtual void end() = 0;
        virtual void bindRenderTarget(GfxRenderPass* renderPass, GfxFramebuffer* framebuffer, const GfxClearValue& clearValue) = 0;
        virtual void unbindRenderTarget() = 0;
        virtual void setViewport(float x, float y, float w, float h) = 0;
        virtual void setScissor(int x, int y, int w, int h) = 0;
        virtual void bindVertexBuffer(GfxBuffer* vertexBuffer, uint32_t offset) = 0;
        virtual void bindIndexBuffer(GfxBuffer* indexBuffer, uint32_t offset, IndexType type) = 0;
        virtual void bindGraphicsPipeline(GfxGraphicsPipeline* pipeline) = 0;
        virtual void bindRootSignature(GfxRootSignature* rootSignature) = 0;
        virtual void bindDescriptorSets(uint32_t setCount, GfxDescriptorSet** sets) = 0;
        virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
        virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) = 0;
        virtual void copyToBuffer(GfxBuffer* srcBuffer, uint32_t srcOffset, GfxBuffer* dstBuffer, uint32_t dstOffset, uint32_t size) = 0;
        virtual void copyToImage(GfxBuffer* srcBuffer, GfxTexture* dstTexture, const GfxCopyToImageHelper& helper) = 0;
        virtual void setBarrier(uint32_t inBufferBarrierCount, GfxBufferBarrier* inBufferBarriers,
                                uint32_t inTextureBarrierCount, GfxTextureBarrier* inTextureBarriers) = 0;
    };
}