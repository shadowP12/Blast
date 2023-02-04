#pragma once
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "GfxDefine.h"
#include <stdlib.h>
#include <vector>
#include <volk.h>

#define VK_ASSERT(x)                                                    \
    do                                                                  \
    {                                                                   \
        if (x != VK_SUCCESS)                                            \
        {                                                               \
            BLAST_LOGE("Vulkan error at %s:%d.\n", __FILE__, __LINE__); \
            abort();                                                    \
        }                                                               \
    } while (0)

// UniformBuffer shift
#define VULKAN_BINDING_SHIFT_B 0

// Texture shift
#define VULKAN_BINDING_SHIFT_T 1000

// UAV shift
#define VULKAN_BINDING_SHIFT_U 2000

// Sampler shift
#define VULKAN_BINDING_SHIFT_S 3000

namespace blast
{
class VulkanDevice;

class VulkanCommandBuffer : public GfxCommandBuffer
{
public:
    VulkanCommandBuffer() = default;

    ~VulkanCommandBuffer() = default;

private:
    friend class VulkanDevice;
    uint32_t idx = 0;
    bool is_copy = false;
};

VkSampleCountFlagBits ToVulkanSampleCount(SampleCount sample_count);

VkFormat ToVulkanFormat(Format format);

Format ToGfxFormat(VkFormat format);

VkImageAspectFlags ToVulkanAspectMask(Format format);

VkImageUsageFlags ToVulkanImageUsage(Format format);

VkAttachmentLoadOp ToVulkanLoadOp(LoadAction op);

VkBlendOp ToVulkanBlendOp(BlendOp op);

VkBlendFactor ToVulkanBlendFactor(BlendConstant factor);

VkStencilOp ToVulkanStencilOp(StencilOp op);

VkCompareOp ToVulkanCompareOp(CompareMode op);

VkPolygonMode ToVulkanFillMode(FillMode mode);

VkCullModeFlagBits ToVulkanCullMode(CullMode mode);

VkFrontFace ToVulkanFrontFace(FrontFace frontFace);

VkFilter ToVulkanFilter(FilterType filter);

VkSamplerMipmapMode ToVulkanMipmapMode(MipmapMode mode);

VkSamplerAddressMode ToVulkanAddressMode(AddressMode mode);

VkShaderStageFlags ToVulkanShaderStages(ShaderStage stages);

VkIndexType ToVulkanIndexType(IndexType type);

VkAccessFlags ToVulkanAccessFlags(ResourceState state);

VkImageLayout ToVulkanImageLayout(ResourceState state);

VkPipelineStageFlags ToPipelineStageFlags(VkAccessFlags access_flags, QueueType queue_type);
}// namespace blast
