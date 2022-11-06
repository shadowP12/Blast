#pragma once
#include "GfxDefine.h"
#include <memory>

namespace blast {
    class GfxDevice {
    public:
        GfxDevice() = default;

        virtual ~GfxDevice() = default;

        static GfxDevice* CreateDevice();

        virtual GfxBuffer* CreateBuffer(const GfxBufferDesc& desc) = 0;

        virtual void DestroyBuffer(GfxBuffer*) = 0;

        virtual GfxTexture* CreateTexture(const GfxTextureDesc& desc) = 0;

        virtual void DestroyTexture(GfxTexture*) = 0;

        virtual int32_t CreateSubresource(GfxTexture*, SubResourceType, uint32_t, uint32_t, uint32_t, uint32_t) = 0;

        virtual GfxSampler* CreateSampler(const GfxSamplerDesc& desc) = 0;

        virtual void DestroySampler(GfxSampler*) = 0;

        virtual GfxSwapChain* CreateSwapChain(const GfxSwapChainDesc& desc, GfxSwapChain* old_swapchain = nullptr) = 0;

        virtual void DestroySwapChain(GfxSwapChain*) = 0;

        virtual GfxRenderPass* CreateRenderPass(const GfxRenderPassDesc& desc) = 0;

        virtual void DestroyRenderPass(GfxRenderPass*) = 0;

        virtual GfxShader* CreateShader(const GfxShaderDesc& desc) = 0;

        virtual void DestroyShader(GfxShader*) = 0;

        virtual GfxPipeline* CreatePipeline(const GfxPipelineDesc& desc) = 0;

        virtual void DestroyPipeline(GfxPipeline*) = 0;

        virtual GfxCommandBuffer* RequestCommandBuffer(QueueType type) = 0;

        virtual void SubmitAllCommandBuffer() = 0;

        virtual void WaitCommandBuffer(GfxCommandBuffer* cmd, GfxCommandBuffer* wait_for) = 0;

        virtual void RenderPassBegin(GfxCommandBuffer* cmd, GfxSwapChain* swapchain) = 0;

        virtual void RenderPassBegin(GfxCommandBuffer* cmd, GfxRenderPass* renderpass) = 0;

        virtual void RenderPassEnd(GfxCommandBuffer* cmd) = 0;

        virtual void BindScissor(GfxCommandBuffer* cmd, int32_t left, int32_t top, int32_t right, int32_t bottom, uint32_t idx = 0) = 0;

        virtual void BindViewport(GfxCommandBuffer* cmd, float x, float y, float w, float h, float min_depth = 0.0f, float max_depth = 1.0f, uint32_t idx = 0) = 0;

        virtual void BindResource(GfxCommandBuffer* cmd, GfxResource* resource, uint32_t slot, int32_t subresource = -1) = 0;

        virtual void BindUAV(GfxCommandBuffer* cmd, GfxResource* resource, uint32_t slot, int32_t subresource = -1) = 0;

        virtual void BindSampler(GfxCommandBuffer* cmd, GfxSampler* sampler, uint32_t slot) = 0;

        virtual void BindConstantBuffer(GfxCommandBuffer* cmd, GfxBuffer* buffer, uint32_t slot, uint64_t size, uint64_t offset = 0) = 0;

        virtual void BindVertexBuffers(GfxCommandBuffer* cmd, GfxBuffer** vertex_buffers, uint32_t slot, uint32_t count, uint64_t* offsets) = 0;

        virtual void BindIndexBuffer(GfxCommandBuffer* cmd, GfxBuffer* index_buffer, IndexType type, uint64_t offset) = 0;

        virtual void BindStencilRef(GfxCommandBuffer* cmd, uint32_t value) = 0;

        virtual void BindBlendFactor(GfxCommandBuffer* cmd, float r, float g, float b, float a) = 0;

        virtual void BindPipeline(GfxCommandBuffer* cmd, GfxPipeline* pso) = 0;

        virtual void BindComputeShader(GfxCommandBuffer* cmd, GfxShader* cs) = 0;

        virtual void PushConstants(GfxCommandBuffer* cmd, const void* data, uint32_t size) = 0;

        virtual void Draw(GfxCommandBuffer* cmd, uint32_t vertex_count, uint32_t vertex_offset) = 0;

        virtual void DrawIndexed(GfxCommandBuffer* cmd, uint32_t index_count, uint32_t index_offset, int32_t vertex_offset) = 0;

        virtual void DrawInstanced(GfxCommandBuffer* cmd, uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset) = 0;

        virtual void DrawIndexedInstanced(GfxCommandBuffer* cmd, uint32_t index_count, uint32_t instance_count, uint32_t index_offset, int32_t vertex_offset, uint32_t instance_offset) = 0;

        virtual void Dispatch(GfxCommandBuffer* cmd, uint32_t thread_group_x, uint32_t thread_group_y, uint32_t thread_group_z) = 0;

        virtual void UpdateBuffer(GfxCommandBuffer* cmd, GfxBuffer* buffer, const void* data, uint64_t size = 0, uint64_t offset = 0) = 0;

        virtual void UpdateTexture(GfxCommandBuffer* cmd, GfxTexture* texture, const void* data, uint32_t layer = 0, uint32_t level = 0) = 0;

        virtual void SetBarrier(GfxCommandBuffer* cmd, uint32_t num_barriers, GfxResourceBarrier* barriers) = 0;

    protected:
        uint64_t frame_count = 0;
    };
}