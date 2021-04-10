#pragma once
#include "VulkanDefine.h"
#include "../GfxBuffer.h"

namespace Blast {
    class VulkanContext;

    class VulkanBuffer : public GfxBuffer {
    public:
        VulkanBuffer(VulkanContext* context, const GfxBufferDesc& desc);
        ~VulkanBuffer();
        void readData(uint32_t offset, uint32_t size, void* dest) override;
        void writeData(uint32_t offset, uint32_t size, void* source) override;
        VkBuffer getHandle() { return mBuffer; }
    protected:
        VulkanContext* mContext = nullptr;
        VkBuffer mBuffer;
        VkDeviceMemory mMemory;
    };
}