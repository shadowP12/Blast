#pragma once
#include "VulkanDefine.h"
#include "../GfxTexture.h"

namespace Blast {
    class VulkanDevice;

    class VulkanTexture : public GfxTexture {
    public:
        VulkanTexture(VulkanDevice *device, const GfxTextureDesc &desc);
        virtual ~VulkanTexture();
    protected:
        VulkanDevice* mDevice = nullptr;
        VkImage mImage;
        VkImageView mView;
        VkDeviceMemory mMemory;
        bool mOwnsImage = false;
    };
}