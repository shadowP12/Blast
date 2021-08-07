#pragma once
#include "VulkanDefine.h"
#include "../GfxBuffer.h"

namespace blast {
    class VulkanContext;

    class VulkanBuffer : public GfxBuffer {
    public:
        VulkanBuffer(VulkanContext* context, const GfxBufferDesc& desc);

        ~VulkanBuffer();

        void ReadData(uint32_t offset, uint32_t size, void* dest) override;

        void WriteData(uint32_t offset, uint32_t size, void* source) override;

        VkBuffer GetHandle() { return _buffer; }

    protected:
        VulkanContext* _context = nullptr;
        VkBuffer _buffer;
        VkDeviceMemory _memory;
    };
}