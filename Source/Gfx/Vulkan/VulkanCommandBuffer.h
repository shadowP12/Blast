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
        VkCommandPool getHandle() { return mPool; }
    protected:
        VulkanContext* mContext = nullptr;
        VkCommandPool mPool;
    };

    class VulkanCommandBuffer : public GfxCommandBuffer {
    public:
        VulkanCommandBuffer(VulkanContext* context, const GfxCommandBufferDesc& desc);
        ~VulkanCommandBuffer();
        VkCommandBuffer getHandle() { return mCommandBuffer; }
    protected:
        VulkanContext* mContext = nullptr;
        VulkanCommandBufferPool* mPool;
        VkCommandBuffer mCommandBuffer;
    };
}