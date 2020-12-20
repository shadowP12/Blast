#include "VulkanBuffer.h"
#include "../GfxDevice.h"
#include "VulkanDevice.h"

namespace Blast {
    VulkanBuffer::VulkanBuffer(VulkanDevice *device, const GfxBufferDesc &desc)
    : GfxBuffe(desc) {
        mDevice = device;

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

        VK_ASSERT(vkCreateBuffer(mDevice->getHandle(), &bufferInfo, nullptr, &mBuffer));

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(mDevice->getHandle(), mBuffer, &memoryRequirements);

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
        memoryAllocateInfo.memoryTypeIndex = mDevice->findMemoryType(memoryRequirements.memoryTypeBits, memoryPropertys);

        VK_ASSERT(vkAllocateMemory(mDevice->getHandle(), &memoryAllocateInfo, nullptr, &mMemory));

        VK_ASSERT(vkBindBufferMemory(mDevice->getHandle(), mBuffer, mMemory, 0));
    }

    VulkanBuffer::~VulkanBuffer() {
        vkDestroyBuffer(mDevice->getHandle(), mBuffer, nullptr);
        vkFreeMemory(mDevice->getHandle(), mMemory, nullptr);
    }

    void VulkanBuffer::readData(uint32_t offset, uint32_t size, void* dest) {
        if (mUsage != RESOURCE_USAGE_GPU_TO_CPU)
            return;

        void* data;
        vkMapMemory(mDevice->getHandle(), mMemory, offset, size, 0, &data);
        memcpy(dest, data, static_cast<size_t>(size));
        vkUnmapMemory(mDevice->getHandle(), mMemory);
    }

    void VulkanBuffer::writeData(uint32_t offset, uint32_t size, void* source) {
        if (mUsage != RESOURCE_USAGE_CPU_TO_GPU)
            return;

        void *data;
        vkMapMemory(mDevice->getHandle(), mMemory, offset, size, 0, &data);
        memcpy(data, source, static_cast<size_t>(size));
        vkUnmapMemory(mDevice->getHandle(), mMemory);
    }
}