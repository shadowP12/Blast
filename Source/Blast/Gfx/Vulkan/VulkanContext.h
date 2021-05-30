#pragma once
#include "../GfxContext.h"
#include "VulkanDefine.h"

namespace Blast {
    class VulkanContext;

    class VulkanSemaphore : public GfxSemaphore {
    public:
        VulkanSemaphore(VulkanContext* context);
        ~VulkanSemaphore();
        VkSemaphore  getHandle() { return mSemaphore; }
    private:
        VulkanContext* mContext = nullptr;
        VkSemaphore  mSemaphore;
    };

    class VulkanFence : public GfxFence {
    public:
        VulkanFence(VulkanContext* context);
        ~VulkanFence();
        FenceStatus getFenceStatus() override;
        void waitForComplete() override;
        VkFence getHandle() { return mFence; }
    private:
        friend class VulkanContext;
        friend class VulkanQueue;
        VulkanContext* mContext = nullptr;
        VkFence mFence = VK_NULL_HANDLE;
        bool mSubmitted = false;
    };

    class VulkanQueue : public GfxQueue {
    public:
        VulkanQueue();
        ~VulkanQueue();
        void submit(const GfxSubmitInfo& info) override;
        int present(const GfxPresentInfo& info) override;
        void waitIdle() override;
        VkQueue  getHandle() { return mQueue; }
        QueueType getType() { return mType; }
        uint32_t getFamilyIndex() { return mFamilyIndex; }
    private:
        friend class VulkanContext;
        VkQueue  mQueue;
        QueueType mType;
        uint32_t mFamilyIndex;
    };

    class VulkanContext : public GfxContext {
    public:
        VulkanContext();
        ~VulkanContext();
        GfxQueue* getQueue(QueueType type) override;
        GfxSemaphore* createSemaphore() override;
        GfxFence* createFence() override;
        GfxBuffer* createBuffer(const GfxBufferDesc& desc) override;
        GfxTexture* createTexture(const GfxTextureDesc& desc) override;
        GfxSampler* createSampler(const GfxSamplerDesc& desc) override;
        GfxSurface* createSurface(const GfxSurfaceDesc& desc) override;
        GfxSwapchain* createSwapchain(const GfxSwapchainDesc& desc) override;
        GfxCommandBufferPool* createCommandBufferPool(const GfxCommandBufferPoolDesc& desc) override;
        GfxRenderPass* createRenderPass(const GfxRenderPassDesc& desc) override;
        GfxFramebuffer* createFramebuffer(const GfxFramebufferDesc& desc) override;
        GfxShader* createShader(const GfxShaderDesc& desc) override;
        GfxRootSignature* createRootSignature(const GfxRootSignatureDesc& desc) override;
        GfxGraphicsPipeline* createGraphicsPipeline(const GfxGraphicsPipelineDesc& desc) override;
        void acquireNextImage(GfxSwapchain* swapchain, GfxSemaphore* signalSemaphore, GfxFence* fence, uint32_t* imageIndex) override;
        VkDevice getDevice() { return mDevice; }
        VkPhysicalDevice getPhyDevice() { return mPhyDevice; }
        VkInstance getInstance() { return mInstance; }
        VkDescriptorPool getDescriptorPool() { return mDescriptorPool; }
        int findMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties);
    private:
        VkInstance mInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT mDebugReportCallback = VK_NULL_HANDLE;
        VkPhysicalDevice mPhyDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties mPhyDeviceProperties;
        VkPhysicalDeviceFeatures mPhyDeviceFeatures;
        VkPhysicalDeviceMemoryProperties mPhyDeviceMemoryProperties;
        VkDevice mDevice = VK_NULL_HANDLE;
        VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
        VulkanQueue* mGraphicsQueue = nullptr;
        VulkanQueue* mComputeQueue = nullptr;
        VulkanQueue* mTransferQueue = nullptr;
    };
}