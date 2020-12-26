#pragma once
#include "VulkanDefine.h"
#include "../GfxBuffer.h"

namespace Blast {
    class VulkanContext;

    class VulkanBuffer : public GfxBuffe {
    public:
        VulkanBuffer(VulkanContext* context, const GfxBufferDesc& desc);
        virtual ~VulkanBuffer();
        virtual void readData(uint32_t offset, uint32_t size, void* dest) override;
        virtual void writeData(uint32_t offset, uint32_t size, void* source) override;
    protected:
        VulkanContext* mContext = nullptr;
        VkBuffer mBuffer;
        VkDeviceMemory mMemory;
    };
}