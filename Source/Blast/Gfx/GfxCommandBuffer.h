#pragma once
#include "GfxDefine.h"

namespace blast {
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

        virtual ~GfxCommandBufferPool() = default;

        virtual GfxCommandBuffer* AllocBuffer(bool secondary) = 0;

        virtual void DeleteBuffer(GfxCommandBuffer* buffer) = 0;

    protected:
    };

    struct GfxCommandBufferDesc {
        GfxCommandBufferPool* pool = nullptr;
        bool secondary = false;
    };

    struct GfxClearValue {
        uint32_t flags = CLEAR_NONE;
        float color[4];
        float depth;
        uint32_t stencil;
    };

    struct GfxCopyToBufferRange {
        uint32_t src_offset;
        uint32_t dst_offset;
        uint32_t size;
    };

    struct GfxCopyToImageRange {
        uint32_t buffer_offset = 0;
        uint32_t level = 0;
        uint32_t layer = 0;
    };

    struct GfxBufferBarrier {
        GfxBuffer* buffer;
        ResourceState new_state;
    };

    struct GfxTextureBarrier {
        GfxTexture* texture;
        ResourceState new_state;
    };

    class GfxCommandBuffer {
    public:
        GfxCommandBuffer(const GfxCommandBufferDesc& desc);

        virtual ~GfxCommandBuffer() = default;

        virtual void Begin() = 0;

        virtual void End() = 0;

        virtual void BindFramebuffer(GfxFramebuffer* framebuffer) = 0;

        virtual void UnbindFramebuffer() = 0;

        virtual void ClearFramebuffer(GfxFramebuffer* framebuffer, const GfxClearValue& clear_value) = 0;

        virtual void SetViewport(float x, float y, float w, float h) = 0;

        virtual void SetScissor(int x, int y, int w, int h) = 0;

        virtual void BindVertexBuffer(GfxBuffer* vertex_buffer, uint32_t offset) = 0;

        virtual void BindIndexBuffer(GfxBuffer* index_buffer, uint32_t offset, IndexType type) = 0;

        virtual void BindGraphicsPipeline(GfxGraphicsPipeline* pipeline) = 0;

        virtual void BindDescriptorSets(GfxRootSignature* root_signature, uint32_t num_sets, GfxDescriptorSet** sets) = 0;

        virtual void Draw(uint32_t num_vertices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_instance) = 0;

        virtual void DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_index, uint32_t vertex_offset, uint32_t first_instance) = 0;

        virtual void CopyToBuffer(GfxBuffer* src_buffer, GfxBuffer* dst_buffer, const GfxCopyToBufferRange& range) = 0;

        virtual void CopyToImage(GfxBuffer* src_buffer, GfxTexture* dst_texture, const GfxCopyToImageRange& range) = 0;

        virtual void SetBarrier(uint32_t num_buffer_barriers, GfxBufferBarrier* buffer_barriers,
                                uint32_t num_texture_barriers, GfxTextureBarrier* texture_barriers) = 0;
    };
}