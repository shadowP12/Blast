#include "VulkanDefine.h"

namespace Blast {
    VkSampleCountFlagBits toVulkanSampleCount(SampleCount sampleCount) {
        VkSampleCountFlagBits result = VK_SAMPLE_COUNT_1_BIT;
        switch (sampleCount) {
            case SAMPLE_COUNT_1:
                result = VK_SAMPLE_COUNT_1_BIT;
                break;
            case SAMPLE_COUNT_2:
                result = VK_SAMPLE_COUNT_2_BIT;
                break;
            case SAMPLE_COUNT_4:
                result = VK_SAMPLE_COUNT_4_BIT;
                break;
            case SAMPLE_COUNT_8:
                result = VK_SAMPLE_COUNT_8_BIT;
                break;
            case SAMPLE_COUNT_16:
                result = VK_SAMPLE_COUNT_16_BIT;
                break;
        }
        return result;
    }

    VkFormat toVulkanFormat(Format format) {
        switch (format) {
            case FORMAT_UNKNOWN:
                return VK_FORMAT_UNDEFINED;
                break;
            case FORMAT_R32G32B32A32_FLOAT:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            case FORMAT_R32G32B32A32_UINT:
                return VK_FORMAT_R32G32B32A32_UINT;
                break;
            case FORMAT_R32G32B32A32_SINT:
                return VK_FORMAT_R32G32B32A32_SINT;
                break;
            case FORMAT_R32G32B32_FLOAT:
                return VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case FORMAT_R32G32B32_UINT:
                return VK_FORMAT_R32G32B32_UINT;
                break;
            case FORMAT_R32G32B32_SINT:
                return VK_FORMAT_R32G32B32_SINT;
                break;
            case FORMAT_R16G16B16A16_FLOAT:
                return VK_FORMAT_R16G16B16A16_SFLOAT;
                break;
            case FORMAT_R16G16B16A16_UNORM:
                return VK_FORMAT_R16G16B16A16_UNORM;
                break;
            case FORMAT_R16G16B16A16_UINT:
                return VK_FORMAT_R16G16B16A16_UINT;
                break;
            case FORMAT_R16G16B16A16_SNORM:
                return VK_FORMAT_R16G16B16A16_SNORM;
                break;
            case FORMAT_R16G16B16A16_SINT:
                return VK_FORMAT_R16G16B16A16_SINT;
                break;
            case FORMAT_R32G32_FLOAT:
                return VK_FORMAT_R32G32_SFLOAT;
                break;
            case FORMAT_R32G32_UINT:
                return VK_FORMAT_R32G32_UINT;
                break;
            case FORMAT_R32G32_SINT:
                return VK_FORMAT_R32G32_SINT;
                break;
            case FORMAT_R32G8X24_TYPELESS:
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
                break;
            case FORMAT_D32_FLOAT_S8X24_UINT:
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
                break;
            case FORMAT_R10G10B10A2_UNORM:
                return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
                break;
            case FORMAT_R10G10B10A2_UINT:
                return VK_FORMAT_A2B10G10R10_UINT_PACK32;
                break;
            case FORMAT_R11G11B10_FLOAT:
                return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
                break;
            case FORMAT_R8G8B8A8_UNORM:
                return VK_FORMAT_R8G8B8A8_UNORM;
                break;
            case FORMAT_R8G8B8A8_UNORM_SRGB:
                return VK_FORMAT_R8G8B8A8_SRGB;
                break;
            case FORMAT_R8G8B8A8_UINT:
                return VK_FORMAT_R8G8B8A8_UINT;
                break;
            case FORMAT_R8G8B8A8_SNORM:
                return VK_FORMAT_R8G8B8A8_SNORM;
                break;
            case FORMAT_R8G8B8A8_SINT:
                return VK_FORMAT_R8G8B8A8_SINT;
                break;
            case FORMAT_R16G16_FLOAT:
                return VK_FORMAT_R16G16_SFLOAT;
                break;
            case FORMAT_R16G16_UNORM:
                return VK_FORMAT_R16G16_UNORM;
                break;
            case FORMAT_R16G16_UINT:
                return VK_FORMAT_R16G16_UINT;
                break;
            case FORMAT_R16G16_SNORM:
                return VK_FORMAT_R16G16_SNORM;
                break;
            case FORMAT_R16G16_SINT:
                return VK_FORMAT_R16G16_SINT;
                break;
            case FORMAT_R32_TYPELESS:
                return VK_FORMAT_D32_SFLOAT;
                break;
            case FORMAT_D32_FLOAT:
                return VK_FORMAT_D32_SFLOAT;
                break;
            case FORMAT_R32_FLOAT:
                return VK_FORMAT_R32_SFLOAT;
                break;
            case FORMAT_R32_UINT:
                return VK_FORMAT_R32_UINT;
                break;
            case FORMAT_R32_SINT:
                return VK_FORMAT_R32_SINT;
                break;
            case FORMAT_R24G8_TYPELESS:
                return VK_FORMAT_D24_UNORM_S8_UINT;
                break;
            case FORMAT_D24_UNORM_S8_UINT:
                return VK_FORMAT_D24_UNORM_S8_UINT;
                break;
            case FORMAT_R8G8_UNORM:
                return VK_FORMAT_R8G8_UNORM;
                break;
            case FORMAT_R8G8_UINT:
                return VK_FORMAT_R8G8_UINT;
                break;
            case FORMAT_R8G8_SNORM:
                return VK_FORMAT_R8G8_SNORM;
                break;
            case FORMAT_R8G8_SINT:
                return VK_FORMAT_R8G8_SINT;
                break;
            case FORMAT_R16_TYPELESS:
                return VK_FORMAT_D16_UNORM;
                break;
            case FORMAT_R16_FLOAT:
                return VK_FORMAT_R16_SFLOAT;
                break;
            case FORMAT_D16_UNORM:
                return VK_FORMAT_D16_UNORM;
                break;
            case FORMAT_R16_UNORM:
                return VK_FORMAT_R16_UNORM;
                break;
            case FORMAT_R16_UINT:
                return VK_FORMAT_R16_UINT;
                break;
            case FORMAT_R16_SNORM:
                return VK_FORMAT_R16_SNORM;
                break;
            case FORMAT_R16_SINT:
                return VK_FORMAT_R16_SINT;
                break;
            case FORMAT_R8_UNORM:
                return VK_FORMAT_R8_UNORM;
                break;
            case FORMAT_R8_UINT:
                return VK_FORMAT_R8_UINT;
                break;
            case FORMAT_R8_SNORM:
                return VK_FORMAT_R8_SNORM;
                break;
            case FORMAT_R8_SINT:
                return VK_FORMAT_R8_SINT;
                break;
            case FORMAT_BC1_UNORM:
                return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
                break;
            case FORMAT_BC1_UNORM_SRGB:
                return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
                break;
            case FORMAT_BC2_UNORM:
                return VK_FORMAT_BC2_UNORM_BLOCK;
                break;
            case FORMAT_BC2_UNORM_SRGB:
                return VK_FORMAT_BC2_SRGB_BLOCK;
                break;
            case FORMAT_BC3_UNORM:
                return VK_FORMAT_BC3_UNORM_BLOCK;
                break;
            case FORMAT_BC3_UNORM_SRGB:
                return VK_FORMAT_BC3_SRGB_BLOCK;
                break;
            case FORMAT_BC4_UNORM:
                return VK_FORMAT_BC4_UNORM_BLOCK;
                break;
            case FORMAT_BC4_SNORM:
                return VK_FORMAT_BC4_SNORM_BLOCK;
                break;
            case FORMAT_BC5_UNORM:
                return VK_FORMAT_BC5_UNORM_BLOCK;
                break;
            case FORMAT_BC5_SNORM:
                return VK_FORMAT_BC5_SNORM_BLOCK;
                break;
            case FORMAT_B8G8R8A8_UNORM:
                return VK_FORMAT_B8G8R8A8_UNORM;
                break;
            case FORMAT_B8G8R8A8_UNORM_SRGB:
                return VK_FORMAT_B8G8R8A8_SRGB;
                break;
            case FORMAT_BC6H_UF16:
                return VK_FORMAT_BC6H_UFLOAT_BLOCK;
                break;
            case FORMAT_BC6H_SF16:
                return VK_FORMAT_BC6H_SFLOAT_BLOCK;
                break;
            case FORMAT_BC7_UNORM:
                return VK_FORMAT_BC7_UNORM_BLOCK;
                break;
            case FORMAT_BC7_UNORM_SRGB:
                return VK_FORMAT_BC7_SRGB_BLOCK;
                break;
            case FORMAT_X8_D24_UNORM_PACK32:
                return VK_FORMAT_X8_D24_UNORM_PACK32;
                break;
            case FORMAT_D32_SFLOAT:
                return VK_FORMAT_D32_SFLOAT;
                break;
            case FORMAT_S8_UINT:
                return VK_FORMAT_S8_UINT;
                break;
            case FORMAT_D16_UNORM_S8_UINT:
                return VK_FORMAT_D16_UNORM_S8_UINT;
                break;
            case FORMAT_D32_SFLOAT_S8_UINT:
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
                break;
        }
        return VK_FORMAT_UNDEFINED;
    }

    VkImageAspectFlags toVulkanAspectMask(Format format) {
        VkImageAspectFlags result = 0;
        switch (format) {
            case FORMAT_D16_UNORM:
            case FORMAT_X8_D24_UNORM_PACK32:
            case FORMAT_D32_SFLOAT:
                result = VK_IMAGE_ASPECT_DEPTH_BIT;
                break;
            case FORMAT_S8_UINT:
                result = VK_IMAGE_ASPECT_STENCIL_BIT;
                break;
            case FORMAT_D16_UNORM_S8_UINT:
            case FORMAT_D24_UNORM_S8_UINT:
            case FORMAT_D32_SFLOAT_S8_UINT:
                result = VK_IMAGE_ASPECT_DEPTH_BIT;
                result |= VK_IMAGE_ASPECT_STENCIL_BIT;
                break;
            default:
                result = VK_IMAGE_ASPECT_COLOR_BIT;
                break;
        }
        return result;
    }

    VkBufferUsageFlags toVulkanBufferUsage(ResourceType type) {
        VkBufferUsageFlags result = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        if (type & RESOURCE_TYPE_RW_BUFFER) {
            result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        if (type & RESOURCE_TYPE_UNIFORM_BUFFER) {
            result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        if (type & RESOURCE_TYPE_VERTEX_BUFFER) {
            result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }

        if (type & RESOURCE_TYPE_INDEX_BUFFER) {
            result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }

        if (type & RESOURCE_TYPE_INDIRECT_BUFFER) {
            result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        return result;
    }

    VkImageUsageFlags toVulkanImageUsage(ResourceType type) {
        VkImageUsageFlags result = 0;
        if (RESOURCE_TYPE_TEXTURE == (type & RESOURCE_TYPE_TEXTURE))
            result |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (RESOURCE_TYPE_RW_TEXTURE == (type & RESOURCE_TYPE_RW_TEXTURE))
            result |= VK_IMAGE_USAGE_STORAGE_BIT;
        return result;
    }
}