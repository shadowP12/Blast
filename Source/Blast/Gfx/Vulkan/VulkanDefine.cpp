#include "VulkanDefine.h"

namespace blast {
    VkSampleCountFlagBits ToVulkanSampleCount(SampleCount sample_count) {
        VkSampleCountFlagBits result = VK_SAMPLE_COUNT_1_BIT;
        switch (sample_count) {
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

    VkFormat ToVulkanFormat(Format format) {
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
            case FORMAT_S8_UINT:
                return VK_FORMAT_S8_UINT;
                break;
            case FORMAT_D16_UNORM_S8_UINT:
                return VK_FORMAT_D16_UNORM_S8_UINT;
                break;
            case FORMAT_D32_FLOAT_S8_UINT:
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
                break;
        }
        return VK_FORMAT_UNDEFINED;
    }

    Format ToGfxFormat(VkFormat format) {
        switch (format) {
            case VK_FORMAT_UNDEFINED:
                return FORMAT_UNKNOWN;
                break;
            case VK_FORMAT_R32G32B32A32_SFLOAT:
                return FORMAT_R32G32B32A32_FLOAT;
                break;
            case VK_FORMAT_R32G32B32A32_UINT:
                return FORMAT_R32G32B32A32_UINT;
                break;
            case VK_FORMAT_R32G32B32A32_SINT:
                return FORMAT_R32G32B32A32_SINT;
                break;
            case VK_FORMAT_R32G32B32_SFLOAT:
                return FORMAT_R32G32B32_FLOAT;
                break;
            case VK_FORMAT_R32G32B32_UINT:
                return FORMAT_R32G32B32_UINT;
                break;
            case VK_FORMAT_R32G32B32_SINT:
                return FORMAT_R32G32B32_SINT;
                break;
            case VK_FORMAT_R16G16B16A16_SFLOAT:
                return FORMAT_R16G16B16A16_FLOAT;
                break;
            case VK_FORMAT_R16G16B16A16_UNORM:
                return FORMAT_R16G16B16A16_UNORM;
                break;
            case VK_FORMAT_R16G16B16A16_UINT:
                return FORMAT_R16G16B16A16_UINT;
                break;
            case VK_FORMAT_R16G16B16A16_SNORM:
                return FORMAT_R16G16B16A16_SNORM;
                break;
            case VK_FORMAT_R16G16B16A16_SINT:
                return FORMAT_R16G16B16A16_SINT;
                break;
            case VK_FORMAT_R32G32_SFLOAT:
                return FORMAT_R32G32_FLOAT;
                break;
            case VK_FORMAT_R32G32_UINT:
                return FORMAT_R32G32_UINT;
                break;
            case VK_FORMAT_R32G32_SINT:
                return FORMAT_R32G32_SINT;
                break;
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
                return FORMAT_R10G10B10A2_UNORM;
                break;
            case VK_FORMAT_A2B10G10R10_UINT_PACK32:
                return FORMAT_R10G10B10A2_UINT;
                break;
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
                return FORMAT_R11G11B10_FLOAT;
                break;
            case VK_FORMAT_R8G8B8A8_UNORM:
                return FORMAT_R8G8B8A8_UNORM;
                break;
            case VK_FORMAT_R8G8B8A8_SRGB:
                return FORMAT_R8G8B8A8_UNORM_SRGB;
                break;
            case VK_FORMAT_R8G8B8A8_UINT:
                return FORMAT_R8G8B8A8_UINT;
                break;
            case VK_FORMAT_R8G8B8A8_SNORM:
                return FORMAT_R8G8B8A8_SNORM;
                break;
            case VK_FORMAT_R8G8B8A8_SINT:
                return FORMAT_R8G8B8A8_SINT;
                break;
            case VK_FORMAT_R16G16_SFLOAT:
                return FORMAT_R16G16_FLOAT;
                break;
            case VK_FORMAT_R16G16_UNORM:
                return FORMAT_R16G16_UNORM;
                break;
            case VK_FORMAT_R16G16_UINT:
                return FORMAT_R16G16_UINT;
                break;
            case VK_FORMAT_R16G16_SNORM:
                return FORMAT_R16G16_SNORM;
                break;
            case VK_FORMAT_R16G16_SINT:
                return FORMAT_R16G16_SINT;
                break;
            case VK_FORMAT_D32_SFLOAT:
                return FORMAT_D32_FLOAT;
                break;
            case VK_FORMAT_R32_SFLOAT:
                return FORMAT_R32_FLOAT;
                break;
            case VK_FORMAT_R32_UINT:
                return FORMAT_R32_UINT;
                break;
            case VK_FORMAT_R32_SINT:
                return FORMAT_R32_SINT;
                break;
            case VK_FORMAT_D24_UNORM_S8_UINT:
                return FORMAT_D24_UNORM_S8_UINT;
                break;
            case VK_FORMAT_R8G8_UNORM:
                return FORMAT_R8G8_UNORM;
                break;
            case VK_FORMAT_R8G8_UINT:
                return FORMAT_R8G8_UINT;
                break;
            case VK_FORMAT_R8G8_SNORM:
                return FORMAT_R8G8_SNORM;
                break;
            case VK_FORMAT_R8G8_SINT:
                return FORMAT_R8G8_SINT;
                break;
            case VK_FORMAT_R16_SFLOAT:
                return FORMAT_R16_FLOAT;
                break;
            case VK_FORMAT_D16_UNORM:
                return FORMAT_D16_UNORM;
                break;
            case VK_FORMAT_R16_UNORM:
                return FORMAT_R16_UNORM;
                break;
            case VK_FORMAT_R16_UINT:
                return FORMAT_R16_UINT;
                break;
            case VK_FORMAT_R16_SNORM:
                return FORMAT_R16_SNORM;
                break;
            case VK_FORMAT_R16_SINT:
                return FORMAT_R16_SINT;
                break;
            case VK_FORMAT_R8_UNORM:
                return FORMAT_R8_UNORM;
                break;
            case VK_FORMAT_R8_UINT:
                return FORMAT_R8_UINT;
                break;
            case VK_FORMAT_R8_SNORM:
                return FORMAT_R8_SNORM;
                break;
            case VK_FORMAT_R8_SINT:
                return FORMAT_R8_SINT;
                break;
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
                return FORMAT_BC1_UNORM;
                break;
            case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
                return FORMAT_BC1_UNORM_SRGB;
                break;
            case VK_FORMAT_BC2_UNORM_BLOCK:
                return FORMAT_BC2_UNORM;
                break;
            case VK_FORMAT_BC2_SRGB_BLOCK:
                return FORMAT_BC2_UNORM_SRGB;
                break;
            case VK_FORMAT_BC3_UNORM_BLOCK:
                return FORMAT_BC3_UNORM;
                break;
            case VK_FORMAT_BC3_SRGB_BLOCK:
                return FORMAT_BC3_UNORM_SRGB;
                break;
            case VK_FORMAT_BC4_UNORM_BLOCK:
                return FORMAT_BC4_UNORM;
                break;
            case VK_FORMAT_BC4_SNORM_BLOCK:
                return FORMAT_BC4_SNORM;
                break;
            case VK_FORMAT_BC5_UNORM_BLOCK:
                return FORMAT_BC5_UNORM;
                break;
            case VK_FORMAT_BC5_SNORM_BLOCK:
                return FORMAT_BC5_SNORM;
                break;
            case VK_FORMAT_B8G8R8A8_UNORM:
                return FORMAT_B8G8R8A8_UNORM;
                break;
            case VK_FORMAT_B8G8R8A8_SRGB:
                return FORMAT_B8G8R8A8_UNORM_SRGB;
                break;
            case VK_FORMAT_BC6H_UFLOAT_BLOCK:
                return FORMAT_BC6H_UF16;
                break;
            case VK_FORMAT_BC6H_SFLOAT_BLOCK:
                return FORMAT_BC6H_SF16;
                break;
            case VK_FORMAT_BC7_UNORM_BLOCK:
                return FORMAT_BC7_UNORM;
                break;
            case VK_FORMAT_BC7_SRGB_BLOCK:
                return FORMAT_BC7_UNORM_SRGB;
                break;
            case VK_FORMAT_X8_D24_UNORM_PACK32:
                return FORMAT_X8_D24_UNORM_PACK32;
                break;
            case VK_FORMAT_S8_UINT:
                return FORMAT_S8_UINT;
                break;
            case VK_FORMAT_D16_UNORM_S8_UINT:
                return FORMAT_D16_UNORM_S8_UINT;
                break;
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return FORMAT_D32_FLOAT_S8_UINT;
                break;
        }
        return FORMAT_UNKNOWN;
    }

    VkImageAspectFlags ToVulkanAspectMask(Format format) {
        VkImageAspectFlags result = 0;
        switch (format) {
            case FORMAT_D16_UNORM:
            case FORMAT_X8_D24_UNORM_PACK32:
            case FORMAT_D32_FLOAT:
                result = VK_IMAGE_ASPECT_DEPTH_BIT;
                break;
            case FORMAT_S8_UINT:
                result = VK_IMAGE_ASPECT_STENCIL_BIT;
                break;
            case FORMAT_D16_UNORM_S8_UINT:
            case FORMAT_D24_UNORM_S8_UINT:
            case FORMAT_D32_FLOAT_S8_UINT:
                result = VK_IMAGE_ASPECT_DEPTH_BIT;
                result |= VK_IMAGE_ASPECT_STENCIL_BIT;
                break;
            default:
                result = VK_IMAGE_ASPECT_COLOR_BIT;
                break;
        }
        return result;
    }

    VkAttachmentLoadOp ToVulkanLoadOp(LoadAction op) {
        VkAttachmentLoadOp result = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        switch (op) {
            case LOAD_DONTCARE:
                result = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                break;
            case LOAD_LOAD:
                result = VK_ATTACHMENT_LOAD_OP_LOAD;
                break;
            case LOAD_CLEAR:
                result = VK_ATTACHMENT_LOAD_OP_CLEAR;
                break;
        }
        return result;
    }

    VkBlendOp ToVulkanBlendOp(BlendOp op) {
        VkBlendOp result;
        switch (op) {
            case BLEND_OP_ADD:
                result = VK_BLEND_OP_ADD;
                break;
            case BLEND_OP_SUBTRACT:
                result = VK_BLEND_OP_SUBTRACT;
                break;
            case BLEND_OP_REV_SUBTRACT:
                result = VK_BLEND_OP_REVERSE_SUBTRACT;
                break;
            case BLEND_OP_MIN:
                result = VK_BLEND_OP_MIN;
                break;
            case BLEND_OP_MAX:
                result = VK_BLEND_OP_MAX;
                break;
        }
        return result;
    }

    VkBlendFactor ToVulkanBlendFactor(BlendConstant factor) {
        VkBlendFactor result;
        switch (factor) {
            case BLEND_ZERO:
                result = VK_BLEND_FACTOR_ZERO;
                break;
            case BLEND_ONE:
                result = VK_BLEND_FACTOR_ONE;
                break;
            case BLEND_SRC_COLOR:
                result = VK_BLEND_FACTOR_SRC_COLOR;
                break;
            case BLEND_ONE_MINUS_SRC_COLOR:
                result = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
                break;
            case BLEND_DST_COLOR:
                result = VK_BLEND_FACTOR_DST_COLOR;
                break;
            case BLEND_ONE_MINUS_DST_COLOR:
                result = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
                break;
            case BLEND_SRC_ALPHA:
                result = VK_BLEND_FACTOR_SRC_ALPHA;
                break;
            case BLEND_ONE_MINUS_SRC_ALPHA:
                result = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                break;
            case BLEND_DST_ALPHA:
                result = VK_BLEND_FACTOR_DST_ALPHA;
                break;
            case BLEND_ONE_MINUS_DST_ALPHA:
                result = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
                break;
            case BLEND_SRC_ALPHA_SATURATE:
                result = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
                break;
            case BLEND_BLEND_FACTOR:
                result = VK_BLEND_FACTOR_CONSTANT_COLOR;
                break;
            case BLEND_ONE_MINUS_BLEND_FACTOR:
                result = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
                break;
        }
        return result;
    }

    VkStencilOp ToVulkanStencilOp(StencilOp op) {
        VkStencilOp result;
        switch (op) {
            case STENCIL_OP_KEEP:
                result = VK_STENCIL_OP_KEEP;
                break;
            case STENCIL_OP_SET_ZERO:
                result = VK_STENCIL_OP_ZERO;
                break;
            case STENCIL_OP_REPLACE:
                result = VK_STENCIL_OP_REPLACE;
                break;
            case STENCIL_OP_INVERT:
                result = VK_STENCIL_OP_INVERT;
                break;
            case STENCIL_OP_INCR:
                result = VK_STENCIL_OP_INCREMENT_AND_WRAP;
                break;
            case STENCIL_OP_DECR:
                result = VK_STENCIL_OP_DECREMENT_AND_WRAP;
                break;
            case STENCIL_OP_INCR_SAT:
                result = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
                break;
            case STENCIL_OP_DECR_SAT:
                result = VK_STENCIL_OP_DECREMENT_AND_CLAMP;
                break;
        }
        return result;
    }

    VkCompareOp ToVulkanCompareOp(CompareMode op) {
        VkCompareOp result;
        switch (op) {
            case COMPARE_NEVER:
                result = VK_COMPARE_OP_NEVER;
                break;
            case COMPARE_LESS:
                result = VK_COMPARE_OP_LESS;
                break;
            case COMPARE_EQUAL:
                result = VK_COMPARE_OP_EQUAL;
                break;
            case COMPARE_LEQUAL:
                result = VK_COMPARE_OP_LESS_OR_EQUAL;
                break;
            case COMPARE_GREATER:
                result = VK_COMPARE_OP_GREATER;
                break;
            case COMPARE_NOTEQUAL:
                result = VK_COMPARE_OP_NOT_EQUAL;
                break;
            case COMPARE_GEQUAL:
                result = VK_COMPARE_OP_GREATER_OR_EQUAL;
                break;
            case COMPARE_ALWAYS:
                result = VK_COMPARE_OP_ALWAYS;
                break;
        }
        return result;
    }

    VkPolygonMode ToVulkanFillMode(FillMode mode) {
        VkPolygonMode result;
        switch (mode) {
            case FILL_SOLID:
                result = VK_POLYGON_MODE_FILL;
                break;
            case FILL_WIREFRAME:
                result = VK_POLYGON_MODE_LINE;
                break;
        }
        return result;
    }

    VkCullModeFlagBits ToVulkanCullMode(CullMode mode) {
        VkCullModeFlagBits result;
        switch (mode) {
            case CULL_NONE:
                result = VK_CULL_MODE_NONE;
                break;
            case CULL_BACK:
                result = VK_CULL_MODE_BACK_BIT;
                break;
            case CULL_FRONT:
                result = VK_CULL_MODE_FRONT_BIT;
                break;
        }
        return result;
    }

    VkFrontFace ToVulkanFrontFace(FrontFace frontFace) {
        VkFrontFace result;
        switch (frontFace) {
            case FRONT_FACE_CCW:
                result = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                break;
            case FRONT_FACE_CW:
                result = VK_FRONT_FACE_CLOCKWISE;
                break;
        }
        return result;
    }

    VkFilter ToVulkanFilter(FilterType filter) {
        VkFilter result;
        switch (filter) {
            case FILTER_NEAREST:
                result = VK_FILTER_NEAREST;
                break;
            case FILTER_LINEAR:
                result = VK_FILTER_LINEAR;
                break;
        }
        return result;
    }

    VkSamplerMipmapMode ToVulkanMipmapMode(MipmapMode mode) {
        VkSamplerMipmapMode result;
        switch (mode) {
            case MIPMAP_MODE_NEAREST:
                result = VK_SAMPLER_MIPMAP_MODE_NEAREST;
                break;
            case MIPMAP_MODE_LINEAR:
                result = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                break;
        }
        return result;
    }

    VkSamplerAddressMode ToVulkanAddressMode(AddressMode mode) {
        VkSamplerAddressMode result;
        switch (mode) {
            case ADDRESS_MODE_MIRROR:
                result = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
                break;
            case ADDRESS_MODE_REPEAT:
                result = VK_SAMPLER_ADDRESS_MODE_REPEAT;
                break;
            case ADDRESS_MODE_CLAMP_TO_EDGE:
                result = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
                break;
            case ADDRESS_MODE_CLAMP_TO_BORDER:
                result = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
                break;
        }
        return result;
    }

    VkShaderStageFlags ToVulkanShaderStages(ShaderStage stages) {
        VkShaderStageFlags result = 0;
        if (SHADER_STAGE_ALL_GRAPHICS == (stages & SHADER_STAGE_ALL_GRAPHICS)) {
            result = VK_SHADER_STAGE_ALL_GRAPHICS;
        }
        else {
            if (SHADER_STAGE_VERT == (stages & SHADER_STAGE_VERT)) {
                result |= VK_SHADER_STAGE_VERTEX_BIT;
            }
            if (SHADER_STAGE_TESC == (stages & SHADER_STAGE_TESC)) {
                result |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            }
            if (SHADER_STAGE_TESE == (stages & SHADER_STAGE_TESE)) {
                result |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            }
            if (SHADER_STAGE_GEOM == (stages & SHADER_STAGE_GEOM)) {
                result |= VK_SHADER_STAGE_GEOMETRY_BIT;
            }
            if (SHADER_STAGE_FRAG == (stages & SHADER_STAGE_FRAG)) {
                result |= VK_SHADER_STAGE_FRAGMENT_BIT;
            }
            if (SHADER_STAGE_COMP == (stages & SHADER_STAGE_COMP)) {
                result |= VK_SHADER_STAGE_COMPUTE_BIT;
            }
        }
        return result;
    }

    VkIndexType ToVulkanIndexType(IndexType type) {
        if (type == INDEX_TYPE_UINT16) {
            return VK_INDEX_TYPE_UINT16;
        }

        if (type == INDEX_TYPE_UINT32) {
            return VK_INDEX_TYPE_UINT32;
        }

        return VK_INDEX_TYPE_UINT32;
    }

    VkAccessFlags ToVulkanAccessFlags(ResourceState state) {
        // note: 通过当前资源的使用状态来决定access flags
        VkAccessFlags ret = 0;
        if (state & RESOURCE_STATE_COPY_SOURCE) {
            ret |= VK_ACCESS_TRANSFER_READ_BIT;
        }
        if (state & RESOURCE_STATE_COPY_DEST) {
            ret |= VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        if (state & RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) {
            ret |= VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        }
        if (state & RESOURCE_STATE_INDEX_BUFFER) {
            ret |= VK_ACCESS_INDEX_READ_BIT;
        }
        if (state & RESOURCE_STATE_UNORDERED_ACCESS) {
            ret |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        }
        if (state & RESOURCE_STATE_INDIRECT_ARGUMENT) {
            ret |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        }
        if (state & RESOURCE_STATE_RENDERTARGET) {
            ret |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }
        if (state & RESOURCE_STATE_DEPTH_WRITE) {
            ret |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }
        if ((state & RESOURCE_STATE_SHADER_RESOURCE) || (state & RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)) {
            ret |= VK_ACCESS_SHADER_READ_BIT;
        }
        if (state & RESOURCE_STATE_PRESENT) {
            ret |= VK_ACCESS_MEMORY_READ_BIT;
        }

        return ret;
    }

    VkImageLayout ToVulkanImageLayout(ResourceState state) {
        // note: 通过当前资源的使用状态来决定image layout
        if (state & RESOURCE_STATE_COPY_SOURCE)
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        if (state & RESOURCE_STATE_COPY_DEST)
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        if (state & RESOURCE_STATE_RENDERTARGET)
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        if (state & RESOURCE_STATE_DEPTH_WRITE)
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        if (state & RESOURCE_STATE_UNORDERED_ACCESS)
            return VK_IMAGE_LAYOUT_GENERAL;

        if ((state & RESOURCE_STATE_SHADER_RESOURCE) || (state & RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE))
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        if (state & RESOURCE_STATE_PRESENT)
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        if (state == RESOURCE_STATE_COMMON)
            return VK_IMAGE_LAYOUT_GENERAL;

        return VK_IMAGE_LAYOUT_UNDEFINED;
    }

    VkPipelineStageFlags ToPipelineStageFlags(VkAccessFlags accessFlags, QueueType queueType) {
        VkPipelineStageFlags flags = 0;

        switch (queueType) {
            case QUEUE_GRAPHICS: {
                if ((accessFlags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
                    flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

                if ((accessFlags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0) {
                    flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
                    flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                    flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
                    flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
                    flags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
                    flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                }

                if ((accessFlags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0)
                    flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

                if ((accessFlags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0)
                    flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

                if ((accessFlags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
                    flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;
            }
            case QUEUE_COMPUTE: {
                if ((accessFlags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0 ||
                    (accessFlags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0 ||
                    (accessFlags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0 ||
                    (accessFlags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
                    return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

                if ((accessFlags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0)
                    flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

                break;
            }
            default:
                return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        }

        if ((accessFlags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT) != 0)
            flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

        if ((accessFlags & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;

        if ((accessFlags & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT)) != 0)
            flags |= VK_PIPELINE_STAGE_HOST_BIT;

        if (flags == 0)
            flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        return flags;
    }
}