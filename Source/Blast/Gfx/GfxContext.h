#pragma once
#include "GfxDefine.h"
#include <memory>

namespace blast {
    struct GfxBufferDesc;
    class GfxBuffer;
    struct GfxTextureDesc;
    class GfxTexture;
    struct GfxTextureViewDesc;
    class GfxTextureView;
    struct GfxSamplerDesc;
    class GfxSampler;
    struct GfxSurfaceDesc;
    class GfxSurface;
    struct GfxSwapchainDesc;
    class GfxSwapchain;
    struct GfxCommandBufferPoolDesc;
    class GfxCommandBufferPool;
    class GfxCommandBuffer;
    struct GfxRenderPassDesc;
    class GfxRenderPass;
    struct GfxFramebufferDesc;
    class GfxFramebuffer;
    struct GfxShaderDesc;
    class GfxShader;
    struct GfxRootSignatureDesc;
    class GfxRootSignature;
    struct GfxGraphicsPipelineDesc;
    class GfxGraphicsPipeline;

    class GfxSemaphore {
    public:
        GfxSemaphore() = default;

        virtual ~GfxSemaphore() = default;
    };

    class GfxFence {
    public:
        GfxFence() = default;

        virtual ~GfxFence() = default;

        virtual FenceStatus GetFenceStatus() = 0;

        virtual void WaitForComplete() = 0;
    };

    struct GfxSubmitInfo {
        uint32_t num_cmd_bufs = 0;
        GfxCommandBuffer** cmd_bufs;
        uint32_t num_wait_semaphores = 0;
        GfxSemaphore** wait_semaphores = nullptr;
        uint32_t num_signal_semaphores = 0;
        GfxSemaphore** signal_semaphores = nullptr;
        GfxFence* signal_fence = nullptr;
    };

    struct GfxPresentInfo {
        GfxSwapchain* swapchain;
        uint32_t num_wait_semaphores = 0;
        GfxSemaphore** wait_semaphores = nullptr;
        uint32_t index;
    };

    class GfxQueue {
    public:
        GfxQueue() = default;

        virtual ~GfxQueue() = default;

        virtual void Submit(const GfxSubmitInfo& info) = 0;

        virtual int Present(const GfxPresentInfo& info) = 0;

        virtual void WaitIdle() = 0;
    };

    class GfxContext {
    public:
        GfxContext() = default;

        virtual ~GfxContext() = default;

        virtual GfxQueue* GetQueue(QueueType type) = 0;

        virtual GfxSemaphore* CreateSemaphore() = 0;

        virtual void DestroySemaphore(GfxSemaphore*) = 0;

        virtual GfxFence* CreateFence() = 0;

        virtual void DestroyFence(GfxFence*) = 0;

        virtual GfxBuffer* CreateBuffer(const GfxBufferDesc& desc) = 0;

        virtual void DestroyBuffer(GfxBuffer*) = 0;

        virtual GfxTexture* CreateTexture(const GfxTextureDesc& desc) = 0;

        virtual void DestroyTexture(GfxTexture*) = 0;

        virtual GfxTextureView* CreateTextureView(const GfxTextureViewDesc& desc) = 0;

        virtual void DestroyTextureView(GfxTextureView*) = 0;

        virtual GfxSampler* CreateSampler(const GfxSamplerDesc& desc) = 0;

        virtual void DestroySampler(GfxSampler*) = 0;

        virtual GfxSurface* CreateSurface(const GfxSurfaceDesc& desc) = 0;

        virtual void DestroySurface(GfxSurface*) = 0;

        virtual GfxSwapchain* CreateSwapchain(const GfxSwapchainDesc& desc) = 0;

        virtual void DestroySwapchain(GfxSwapchain*) = 0;

        virtual GfxCommandBufferPool* CreateCommandBufferPool(const GfxCommandBufferPoolDesc& desc) = 0;

        virtual void DestroyCommandBufferPool(GfxCommandBufferPool*) = 0;

        virtual GfxFramebuffer* CreateFramebuffer(const GfxFramebufferDesc& desc) = 0;

        virtual void DestroyFramebuffer(GfxFramebuffer*) = 0;

        virtual GfxShader* CreateShader(const GfxShaderDesc& desc) = 0;

        virtual void DestroyShader(GfxShader*) = 0;

        virtual GfxRootSignature* CreateRootSignature(const GfxRootSignatureDesc& desc) = 0;

        virtual void DestroyRootSignature(GfxRootSignature*) = 0;

        virtual GfxGraphicsPipeline* CreateGraphicsPipeline(const GfxGraphicsPipelineDesc& desc) = 0;

        virtual void DestroyGraphicsPipeline(GfxGraphicsPipeline*) = 0;

        virtual void AcquireNextImage(GfxSwapchain* swapchain, GfxSemaphore* signal_semaphore, GfxFence* fence, uint32_t* image_index) = 0;
    };
}