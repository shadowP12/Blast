#include "VulkanTexture.h"
#include "VulkanContext.h"
#include <assert.h>

namespace blast {
    VulkanTexture::VulkanTexture(VulkanContext* context, const GfxTextureDesc& desc)
    :GfxTexture(desc) {
        _own_image = true;
        _context = context;

        VkImageType image_type = VK_IMAGE_TYPE_MAX_ENUM;
        if (_depth > 1)
            image_type = VK_IMAGE_TYPE_3D;
        else if (_height > 1)
            image_type = VK_IMAGE_TYPE_2D;
        else
            image_type = VK_IMAGE_TYPE_1D;

        VkImageCreateInfo ici = {};
        ici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ici.pNext = nullptr;
        ici.flags = 0;
        ici.imageType = image_type;
        ici.format = ToVulkanFormat(_format);
        ici.extent.width = _width;
        ici.extent.height = _height;
        ici.extent.depth = _depth;
        ici.mipLevels = _num_mips;
        ici.arrayLayers = _num_layers;
        ici.samples = ToVulkanSampleCount(_sample_count);
        ici.usage = ToVulkanImageUsage(_type) | ToVulkanImageUsage(_format);
        ici.tiling = VK_IMAGE_TILING_OPTIMAL;
        ici.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ici.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        ici.queueFamilyIndexCount = 0;
        ici.pQueueFamilyIndices = nullptr;

        if (RESOURCE_TYPE_TEXTURE_CUBE == (_type & RESOURCE_TYPE_TEXTURE_CUBE)) {
            ici.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }
        if (image_type == VK_IMAGE_TYPE_3D)
            ici.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR;

        if ((VK_IMAGE_USAGE_SAMPLED_BIT & ici.usage) || (VK_IMAGE_USAGE_STORAGE_BIT & ici.usage)) {
            ici.usage |= (VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        }

        VK_ASSERT(vkCreateImage(_context->GetDevice(), &ici, nullptr, &_image));

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements(_context->GetDevice(), _image, &memory_requirements);

        VkMemoryPropertyFlags memory_propertys;
        if (_usage == RESOURCE_USAGE_GPU_ONLY)
            memory_propertys = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        if (_usage == RESOURCE_USAGE_CPU_TO_GPU)
            memory_propertys = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        if (_usage == RESOURCE_USAGE_GPU_TO_CPU)
            memory_propertys = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        VkMemoryAllocateInfo mai = {};
        mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mai.allocationSize = memory_requirements.size;
        mai.memoryTypeIndex = _context->FindMemoryType(memory_requirements.memoryTypeBits, memory_propertys);

        VK_ASSERT(vkAllocateMemory(_context->GetDevice(), &mai, nullptr, &_memory));

        VK_ASSERT(vkBindImageMemory(_context->GetDevice(), _image, _memory, 0));
    }

    VulkanTexture::VulkanTexture(VulkanContext* context, const GfxTextureDesc& desc, const VkImage& image)
    :GfxTexture(desc) {
        _context = context;
        _image = image;
        _own_image = false;
    }

    VulkanTexture::~VulkanTexture() {
        if(_own_image) {
            vkDestroyImage(_context->GetDevice(), _image, nullptr);
            vkFreeMemory(_context->GetDevice(), _memory, nullptr);
        }
    }

    VulkanTextureView::VulkanTextureView(VulkanContext* context, const GfxTextureViewDesc& desc)
    : GfxTextureView(desc) {
        _context = context;
        _texture = (VulkanTexture*)desc.texture;

        VulkanTexture* internel_texture = _texture;

        VkImageType image_type = VK_IMAGE_TYPE_MAX_ENUM;
        if (internel_texture->_depth > 1)
            image_type = VK_IMAGE_TYPE_3D;
        else if (internel_texture->_height > 1)
            image_type = VK_IMAGE_TYPE_2D;
        else
            image_type = VK_IMAGE_TYPE_1D;

        VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
        switch (image_type) {
            case VK_IMAGE_TYPE_1D:
                view_type = internel_texture->_num_layers > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
                break;
            case VK_IMAGE_TYPE_2D:
                if (RESOURCE_TYPE_TEXTURE_CUBE == (internel_texture->_type & RESOURCE_TYPE_TEXTURE_CUBE))
                    view_type = (internel_texture->_num_layers > 6) ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
                else
                    view_type = internel_texture->_num_layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
                break;
            case VK_IMAGE_TYPE_3D:
                if (internel_texture->_num_layers > 1)
                    assert(false);

                view_type = VK_IMAGE_VIEW_TYPE_3D;
                break;
            default:
                assert(false && "image format not supported!");
                break;
        }

        VkImageAspectFlags aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
        if (ToVulkanAspectMask(internel_texture->_format) & VK_IMAGE_ASPECT_DEPTH_BIT) {
            aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
        }

        VkImageViewCreateInfo ivci = {};
        ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivci.image = internel_texture->_image;
        ivci.viewType = view_type;
        ivci.format = ToVulkanFormat(internel_texture->_format);
        ivci.subresourceRange.aspectMask = aspect_mask;
        ivci.subresourceRange.baseMipLevel = _level;
        ivci.subresourceRange.levelCount = internel_texture->_num_mips;
        ivci.subresourceRange.baseArrayLayer = _layer;
        ivci.subresourceRange.layerCount = internel_texture->_num_layers;
        VK_ASSERT(vkCreateImageView(_context->GetDevice(), &ivci, nullptr, &_view));
    }

    VulkanTextureView::~VulkanTextureView() {
        vkDestroyImageView(_context->GetDevice(), _view, nullptr);
    }

}