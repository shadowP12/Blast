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
        void reset() override;
        VkFence getHandle() { return mFence; }
    private:
        VulkanContext* mContext = nullptr;
        VkFence mFence = VK_NULL_HANDLE;
    };

    class VulkanQueue : public GfxQueue {
    public:
        VulkanQueue();
        ~VulkanQueue();
        void submit(const GfxSubmitInfo& info) override;
        void present(const GfxPresentInfo& info) override;
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
        virtual ~VulkanContext();
        GfxQueue* getQueue(QueueType type) override;
        GfxSemaphore* createSemaphore() override;
        GfxFence* createFence() override;
        GfxBuffer* createBuffer(const GfxBufferDesc& desc) override;
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