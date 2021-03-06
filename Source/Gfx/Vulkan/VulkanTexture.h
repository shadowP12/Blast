#pragma once
#include "VulkanDefine.h"
#include "../GfxTexture.h"

namespace Blast {
    class VulkanContext;

    class VulkanTexture : public GfxTexture {
    public:
        VulkanTexture(VulkanContext* context, const GfxTextureDesc& desc);
        VulkanTexture(VulkanContext* context, const VkImage& image, const GfxTextureDesc& desc);
        ~VulkanTexture();
        VkImage getImage() { return mImage; }
        VkImageView getView() { return mView; }
    protected:
        VulkanContext* mContext = nullptr;
        VkImage mImage;
        VkImageView mView;
        VkDeviceMemory mMemory;
        bool mOwnsImage = false;
    };
}