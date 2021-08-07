#pragma once
#include "../GfxContext.h"
#include "VulkanDefine.h"

namespace blast {
    class VulkanContext;

    class VulkanSemaphore : public GfxSemaphore {
    public:
        VulkanSemaphore(VulkanContext* context);

        ~VulkanSemaphore();

        VkSemaphore  GetHandle() { return _semaphore; }

    private:
        VulkanContext* _context = nullptr;
        VkSemaphore  _semaphore;
    };

    class VulkanFence : public GfxFence {
    public:
        VulkanFence(VulkanContext* context);

        ~VulkanFence();

        FenceStatus GetFenceStatus() override;

        void WaitForComplete() override;

        VkFence GetHandle() { return _fence; }

    private:
        friend class VulkanContext;
        friend class VulkanQueue;
        VulkanContext* _context = nullptr;
        VkFence _fence;
        bool _submitted = false;
    };

    class VulkanQueue : public GfxQueue {
    public:
        VulkanQueue();

        ~VulkanQueue();

        void Submit(const GfxSubmitInfo& info) override;

        int Present(const GfxPresentInfo& info) override;

        void WaitIdle() override;

        VkQueue GetHandle() { return _queue; }

        QueueType GetType() { return _type; }

        uint32_t GetFamilyIndex() { return _family_index; }

    private:
        friend class VulkanContext;
        VkQueue _queue;
        QueueType _type;
        uint32_t _family_index;
    };

    class VulkanContext : public GfxContext {
    public:
        VulkanContext();

        ~VulkanContext();

        GfxQueue* GetQueue(QueueType type) override;

        GfxSemaphore* CreateSemaphore() override;

        void DestroySemaphore(GfxSemaphore*) override;

        GfxFence* CreateFence() override;

        void DestroyFence(GfxFence*) override;

        GfxBuffer* CreateBuffer(const GfxBufferDesc& desc) override;

        void DestroyBuffer(GfxBuffer*) override;

        GfxTexture* CreateTexture(const GfxTextureDesc& desc) override;

        void DestroyTexture(GfxTexture*) override;

        GfxTextureView* CreateTextureView(const GfxTextureViewDesc& desc) override;

        void DestroyTextureView(GfxTextureView*) override;

        GfxSampler* CreateSampler(const GfxSamplerDesc& desc) override;

        void DestroySampler(GfxSampler*) override;

        GfxSurface* CreateSurface(const GfxSurfaceDesc& desc) override;

        void DestroySurface(GfxSurface*) override;

        GfxSwapchain* CreateSwapchain(const GfxSwapchainDesc& desc) override;

        void DestroySwapchain(GfxSwapchain*) override;

        GfxCommandBufferPool* CreateCommandBufferPool(const GfxCommandBufferPoolDesc& desc) override;

        void DestroyCommandBufferPool(GfxCommandBufferPool*) override;

        GfxFramebuffer* CreateFramebuffer(const GfxFramebufferDesc& desc) override;

        void DestroyFramebuffer(GfxFramebuffer*) override;

        GfxShader* CreateShader(const GfxShaderDesc& desc) override;

        void DestroyShader(GfxShader*) override;

        GfxRootSignature* CreateRootSignature(const GfxRootSignatureDesc& desc) override;

        void DestroyRootSignature(GfxRootSignature*) override;

        GfxGraphicsPipeline* CreateGraphicsPipeline(const GfxGraphicsPipelineDesc& desc) override;

        void DestroyGraphicsPipeline(GfxGraphicsPipeline*) override;

        void AcquireNextImage(GfxSwapchain* swapchain, GfxSemaphore* signal_semaphore, GfxFence* fence, uint32_t* image_index) override;

        VkDevice GetDevice() { return _device; }

        VkPhysicalDevice GetPhyDevice() { return _phy_device; }

        VkInstance GetInstance() { return _instance; }

        VkDescriptorPool GetDescriptorPool() { return _descriptor_pool; }

        uint32_t FindMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties);

    private:
        VkInstance _instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT _debug_messenger = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT _debug_report_callback = VK_NULL_HANDLE;
        VkPhysicalDevice _phy_device = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties _phy_device_properties;
        VkPhysicalDeviceFeatures _phy_device_features;
        VkPhysicalDeviceMemoryProperties _phy_device_memory_properties;
        VkDevice _device = VK_NULL_HANDLE;
        VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;
        VulkanQueue* _graphics_queue = nullptr;
        VulkanQueue* _compute_queue = nullptr;
        VulkanQueue* _transfer_queue = nullptr;
    };
}