#include "VulkanBuffer.h"
#include "../GfxContext.h"
#include "VulkanContext.h"

namespace blast {
    VulkanBuffer::VulkanBuffer(VulkanContext* context, const GfxBufferDesc& desc)
    : GfxBuffer(desc) {
        _context = context;

        VkBufferCreateInfo bci;
        bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bci.pNext = nullptr;
        bci.flags = 0;
        bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bci.usage = ToVulkanBufferUsage(_type);
        bci.queueFamilyIndexCount = 0;
        bci.pQueueFamilyIndices = nullptr;
        bci.size = _size;
        if (_usage == RESOURCE_USAGE_GPU_ONLY || _usage == RESOURCE_USAGE_GPU_TO_CPU)
            bci.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VK_ASSERT(vkCreateBuffer(_context->GetDevice(), &bci, nullptr, &_buffer));

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(_context->GetDevice(), _buffer, &memory_requirements);

        VkMemoryPropertyFlags memory_propertys;
        if (_usage == RESOURCE_USAGE_GPU_ONLY)
            memory_propertys = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        if (_usage == RESOURCE_USAGE_CPU_TO_GPU)
            memory_propertys = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        if (_usage == RESOURCE_USAGE_GPU_TO_CPU)
            memory_propertys = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VkMemoryAllocateInfo memory_allocate_info = {};
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.allocationSize = memory_requirements.size;
        memory_allocate_info.memoryTypeIndex = _context->FindMemoryType(memory_requirements.memoryTypeBits, memory_propertys);

        VK_ASSERT(vkAllocateMemory(_context->GetDevice(), &memory_allocate_info, nullptr, &_memory));

        VK_ASSERT(vkBindBufferMemory(_context->GetDevice(), _buffer, _memory, 0));
    }

    VulkanBuffer::~VulkanBuffer() {
        vkDestroyBuffer(_context->GetDevice(), _buffer, nullptr);
        vkFreeMemory(_context->GetDevice(), _memory, nullptr);
    }

    void VulkanBuffer::ReadData(uint32_t offset, uint32_t size, void* dest) {
        void* data;
        vkMapMemory(_context->GetDevice(), _memory, offset, size, 0, &data);
        memcpy(dest, data, static_cast<size_t>(size));
        vkUnmapMemory(_context->GetDevice(), _memory);
    }

    void VulkanBuffer::WriteData(uint32_t offset, uint32_t size, void* source) {
        void *data;
        vkMapMemory(_context->GetDevice(), _memory, offset, size, 0, &data);
        memcpy(data, source, static_cast<size_t>(size));
        vkUnmapMemory(_context->GetDevice(), _memory);
    }
}