#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"

namespace Blast {
    VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanContext* context, const GfxCommandBufferPoolDesc& desc)
        :GfxCommandBufferPool(desc) {
        mContext = context;
        VulkanQueue* queue = static_cast<VulkanQueue*>(desc.queue);
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queue->getFamilyIndex();
        if (!desc.transient)
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        else
            poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        VK_ASSERT(vkCreateCommandPool(mContext->getDevice(), &poolInfo, nullptr, &mPool));
    }

    VulkanCommandBufferPool::~VulkanCommandBufferPool() {
        vkDestroyCommandPool(mContext->getDevice(), mPool, nullptr);
    }

    GfxCommandBuffer * VulkanCommandBufferPool::allocBuf(bool secondary) {
        GfxCommandBufferDesc desc;
        desc.pool = this;
        desc.secondary = secondary;
        VulkanCommandBuffer* cmd = new VulkanCommandBuffer(mContext, desc);
        return cmd;
    }

    VulkanCommandBuffer::VulkanCommandBuffer(VulkanContext* context, const GfxCommandBufferDesc& desc)
        :GfxCommandBuffer(desc) {
        mContext = context;
        mPool = static_cast<VulkanCommandBufferPool*>(desc.pool);

        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = mPool->getHandle();
        allocateInfo.commandBufferCount = 1;
        if (!desc.secondary)
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        else
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;

        VK_ASSERT(vkAllocateCommandBuffers(mContext->getDevice(), &allocateInfo, &mCommandBuffer));
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() {
        vkFreeCommandBuffers(mContext->getDevice(), mPool->getHandle(), 1, &mCommandBuffer);
    }
}