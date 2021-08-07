#pragma once
#include "VulkanDefine.h"
#include "../GfxTexture.h"
#include <vector>

namespace blast {
    class VulkanContext;

    class VulkanTexture : public GfxTexture {
    public:
        VulkanTexture(VulkanContext* context, const GfxTextureDesc& desc);

        VulkanTexture(VulkanContext* context, const GfxTextureDesc& desc, const VkImage& image);

        ~VulkanTexture();

        VkImage GetHandle() { return _image; }

    protected:
        friend class VulkanTextureView;
        VulkanContext* _context = nullptr;
        VkImage _image;
        VkDeviceMemory _memory;
        bool _own_image;
    };

    class VulkanTextureView : public GfxTextureView {
    public:
        VulkanTextureView(VulkanContext* context, const GfxTextureViewDesc& desc);

        ~VulkanTextureView();

        VkImageView GetHandle() { return _view; }

        VulkanTexture* GetTexture() { return _texture; }

    protected:
        VulkanContext* _context = nullptr;
        VulkanTexture* _texture = nullptr;
        VkImageView _view;
    };
}