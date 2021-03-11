#pragma once
#include "VulkanDefine.h"
#include "../GfxTexture.h"
#include <vector>

namespace Blast {
    class VulkanContext;

    class VulkanTexture : public GfxTexture {
    public:
        VulkanTexture(VulkanContext* context, const GfxTextureDesc& desc);
        VulkanTexture(VulkanContext* context, const VkImage& image, const GfxTextureDesc& desc);
        ~VulkanTexture();
        VkImage getImage() { return mImage; }
        VkImageView getSRV(uint32_t layer, uint32_t level) { return mSRVs[layer][level]; }
        VkImageView getUAV(uint32_t layer, uint32_t level) { return mUAVs[layer][level]; }
    protected:
        VulkanContext* mContext = nullptr;
        VkImage mImage;
        std::vector<std::vector<VkImageView>> mSRVs;
        std::vector<std::vector<VkImageView>> mUAVs;
        VkDeviceMemory mMemory;
        bool mOwnsImage = false;
    };
}