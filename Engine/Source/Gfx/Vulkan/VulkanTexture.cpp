#include "VulkanTexture.h"
#include "VulkanDevice.h"
#include <assert.h>

namespace Blast {
    VulkanTexture::VulkanTexture(VulkanDevice *device, const GfxTextureDesc &desc)
    :GfxTexture(desc) {
        mDevice = device;
        mOwnsImage = true;

        bool cubemapRequired = false;

        VkImageUsageFlags additionalFlags = 0;
        if (desc.colorAtt)
            additionalFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        else if (desc.depthStencilAtt)
            additionalFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VkImageType imageType = VK_IMAGE_TYPE_MAX_ENUM;
        if (mDepth > 1)
            imageType = VK_IMAGE_TYPE_3D;
        else if (mHeight > 1)
            imageType = VK_IMAGE_TYPE_2D;
        else
            imageType = VK_IMAGE_TYPE_1D;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.pNext = nullptr;
        imageInfo.flags = 0;
        imageInfo.imageType = imageType;
        imageInfo.format = toVulkanFormat(mFormat);
        imageInfo.extent.width = mWidth;
        imageInfo.extent.height = mHeight;
        imageInfo.extent.depth = mDepth;
        imageInfo.mipLevels = mMipLevels;
        imageInfo.arrayLayers = mArrayLayers;
        imageInfo.samples = toVulkanSampleCount(mSampleCount);
        imageInfo.usage = toVulkanImageUsage(mType) | additionalFlags;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.queueFamilyIndexCount = 0;
        imageInfo.pQueueFamilyIndices = nullptr;

        if (RESOURCE_TYPE_TEXTURE_CUBE == (mType & RESOURCE_TYPE_TEXTURE_CUBE)) {
            cubemapRequired = true;
            imageInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }
        if (imageType == VK_IMAGE_TYPE_3D)
            imageInfo.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR;

        if ((VK_IMAGE_USAGE_SAMPLED_BIT & imageInfo.usage) || (VK_IMAGE_USAGE_STORAGE_BIT & imageInfo.usage))
        {
            imageInfo.usage |= (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        }

        VK_ASSERT(vkCreateImage(mDevice->getHandle(), &imageInfo, nullptr, &mImage));

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(mDevice->getHandle(), mImage, &memoryRequirements);

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

        VK_ASSERT(vkBindImageMemory(mDevice->getHandle(), mImage, mMemory, 0));

        // create image view
        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
        switch (imageType) {
            case VK_IMAGE_TYPE_1D:
                viewType = mArrayLayers > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
                break;
            case VK_IMAGE_TYPE_2D:
                if (cubemapRequired)
                    viewType = (mArrayLayers > 6) ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
                else
                    viewType = mArrayLayers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
                break;
            case VK_IMAGE_TYPE_3D:
                if (mArrayLayers > 1)
                    assert(false);

                viewType = VK_IMAGE_VIEW_TYPE_3D;
                break;
            default:
                assert(false && "image format not supported!");
                break;
        }
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = mImage;
        viewInfo.viewType = viewType;
        viewInfo.format = toVulkanFormat(mFormat);
        viewInfo.subresourceRange.aspectMask = toVulkanAspectMask(mFormat);
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mMipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = mArrayLayers;

        VK_ASSERT(vkCreateImageView(mDevice->getHandle(), &viewInfo, nullptr, &mView));
    }

    VulkanTexture::~VulkanTexture() {
        if(mOwnsImage) {
            vkDestroyImage(mDevice->getHandle(), mImage, nullptr);
            vkFreeMemory(mDevice->getHandle(), mMemory, nullptr);
        }
        vkDestroyImageView(mDevice->getHandle(), mView, nullptr);
    }
}