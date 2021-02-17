#include "VulkanBuffer.h"
#include "../GfxContext.h"
#include "VulkanContext.h"

namespace Blast {
    VulkanBuffer::VulkanBuffer(VulkanContext* context, const GfxBufferDesc& desc)
    : GfxBuffe(desc) {
        mContext = context;

        VkBufferCreateInfo bufferInfo;
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.flags = 0;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.usage = toVulkanBufferUsage(mType);
        bufferInfo.queueFamilyIndexCount = 0;
        bufferInfo.pQueueFamilyIndices = nullptr;
        bufferInfo.size = mSize;
        if (mUsage == RESOURCE_USAGE_GPU_ONLY || mUsage == RESOURCE_USAGE_GPU_TO_CPU)
            bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VK_ASSERT(vkCreateBuffer(mContext->getDevice(), &bufferInfo, nullptr, &mBuffer));

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(mContext->getDevice(), mBuffer, &memoryRequirements);

        VkMemoryPropertyFlags memoryPropertys;
        if (mUsage == RESOURCE_USAGE_GPU_ONLY)
            memoryPropertys = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        if (mUsage == RESOURCE_USAGE_CPU_TO_GPU)
            memoryPropertys = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        if (mUsage == RESOURCE_USAGE_GPU_TO_CPU)
            memoryPropertys = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = mContext->findMemoryType(memoryRequirements.memoryTypeBits, memoryPropertys);

        VK_ASSERT(vkAllocateMemory(mContext->getDevice(), &memoryAllocateInfo, nullptr, &mMemory));

        VK_ASSERT(vkBindBufferMemory(mContext->getDevice(), mBuffer, mMemory, 0));
    }

    VulkanBuffer::~VulkanBuffer() {
        vkDestroyBuffer(mContext->getDevice(), mBuffer, nullptr);
        vkFreeMemory(mContext->getDevice(), mMemory, nullptr);
    }

    void VulkanBuffer::readData(uint32_t offset, uint32_t size, void* dest) {
        if (mUsage != RESOURCE_USAGE_GPU_TO_CPU)
            return;

        void* data;
        vkMapMemory(mContext->getDevice(), mMemory, offset, size, 0, &data);
        memcpy(dest, data, static_cast<size_t>(size));
        vkUnmapMemory(mContext->getDevice(), mMemory);
    }

    void VulkanBuffer::writeData(uint32_t offset, uint32_t size, void* source) {
        if (mUsage != RESOURCE_USAGE_CPU_TO_GPU)
            return;

        void *data;
        vkMapMemory(mContext->getDevice(), mMemory, offset, size, 0, &data);
        memcpy(data, source, static_cast<size_t>(size));
        vkUnmapMemory(mContext->getDevice(), mMemory);
    }
}