#pragma once
#include "GfxDefine.h"
#include <memory>
namespace Blast {

    struct GfxBufferDesc;
    class GfxBuffer;
    class GfxSwapchainDesc;
    class GfxSwapchain;
    class GfxCommandBuffer;

    class GfxSemaphore {
    public:
    };

    class GfxFence {
    public:
        virtual FenceStatus getFenceStatus() = 0;
        virtual void waitForComplete() = 0;
        virtual void reset() = 0;
    };

    struct GfxSubmitInfo {
        GfxCommandBuffer* cmdBuf;
        uint32_t waitSemaphoreCount = 0;
        GfxSemaphore** waitSemaphores = nullptr;
        uint32_t signalSemaphoreCount = 0;
        GfxSemaphore** signalSemaphores = nullptr;
        GfxFence* signalFence = nullptr;
    };

    struct GfxPresentInfo {
        GfxSwapchain* swapChain;
        uint32_t waitSemaphoreCount = 0;
        GfxSemaphore** waitSemaphores = nullptr;
        uint32_t index;
    };

    class GfxQueue {
    public:
        virtual void submit(const GfxSubmitInfo& info) = 0;
        virtual void present(const GfxPresentInfo& info) = 0;
        virtual void waitIdle() = 0;
    };

    class GfxContext {
    public:
        uint32_t getFormatStride(Format format);
        virtual GfxQueue* getQueue(QueueType type) = 0;
        virtual GfxSemaphore* createSemaphore() = 0;
        virtual GfxFence* createFence() = 0;
        virtual GfxBuffer* createBuffer(const GfxBufferDesc& desc) = 0;
        virtual void acquireNextImage(GfxSwapchain* swapchain, GfxSemaphore* signalSemaphore, GfxFence* fence, uint32_t* imageIndex) = 0;
    };
}