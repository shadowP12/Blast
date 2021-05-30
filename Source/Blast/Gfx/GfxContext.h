#pragma once
#include "GfxDefine.h"
#include <memory>
namespace Blast {
    struct GfxBufferDesc;
    class GfxBuffer;
    struct GfxTextureDesc;
    class GfxTexture;
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
        virtual FenceStatus getFenceStatus() = 0;
        virtual void waitForComplete() = 0;
        // 不应该把状态接口暴露给外部
        // virtual void reset() = 0;
    };

    struct GfxSubmitInfo {
        uint32_t cmdBufCount = 0;
        GfxCommandBuffer** cmdBufs;
        uint32_t waitSemaphoreCount = 0;
        GfxSemaphore** waitSemaphores = nullptr;
        uint32_t signalSemaphoreCount = 0;
        GfxSemaphore** signalSemaphores = nullptr;
        GfxFence* signalFence = nullptr;
    };

    struct GfxPresentInfo {
        GfxSwapchain* swapchain;
        uint32_t waitSemaphoreCount = 0;
        GfxSemaphore** waitSemaphores = nullptr;
        uint32_t index;
    };

    class GfxQueue {
    public:
        GfxQueue() = default;
        virtual ~GfxQueue() = default;
        virtual void submit(const GfxSubmitInfo& info) = 0;
        virtual int present(const GfxPresentInfo& info) = 0;
        virtual void waitIdle() = 0;
    };

    class GfxContext {
    public:
        GfxContext() = default;
        virtual ~GfxContext() = default;
        uint32_t getFormatStride(Format format);
        virtual GfxQueue* getQueue(QueueType type) = 0;
        virtual GfxSemaphore* createSemaphore() = 0;
        virtual GfxFence* createFence() = 0;
        virtual GfxBuffer* createBuffer(const GfxBufferDesc& desc) = 0;
        virtual GfxTexture* createTexture(const GfxTextureDesc& desc) = 0;
        virtual GfxSampler* createSampler(const GfxSamplerDesc& desc) = 0;
        virtual GfxSurface* createSurface(const GfxSurfaceDesc& desc) = 0;
        virtual GfxSwapchain* createSwapchain(const GfxSwapchainDesc& desc) = 0;
        virtual GfxCommandBufferPool* createCommandBufferPool(const GfxCommandBufferPoolDesc& desc) = 0;
        virtual GfxRenderPass* createRenderPass(const GfxRenderPassDesc& desc) = 0;
        virtual GfxFramebuffer* createFramebuffer(const GfxFramebufferDesc& desc) = 0;
        virtual GfxShader* createShader(const GfxShaderDesc& desc) = 0;
        virtual GfxRootSignature* createRootSignature(const GfxRootSignatureDesc& desc) = 0;
        virtual GfxGraphicsPipeline* createGraphicsPipeline(const GfxGraphicsPipelineDesc& desc) = 0;
        virtual void acquireNextImage(GfxSwapchain* swapchain, GfxSemaphore* signalSemaphore, GfxFence* fence, uint32_t* imageIndex) = 0;
    };
}