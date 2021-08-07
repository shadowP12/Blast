#pragma once
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <volk.h>
#include <stdlib.h>
#include "../GfxDefine.h"

#define VK_ASSERT(x)                                                    \
	do                                                                  \
	{                                                                   \
		if (x != VK_SUCCESS)                                            \
		{                                                               \
			BLAST_LOGE("Vulkan error at %s:%d.\n", __FILE__, __LINE__); \
			abort();                                                    \
		}                                                               \
	} while (0)

namespace blast {
    VkSampleCountFlagBits ToVulkanSampleCount(SampleCount sample_count);

    VkFormat ToVulkanFormat(Format format);

    Format ToGfxFormat(VkFormat format);

    VkImageAspectFlags ToVulkanAspectMask(Format format);

    VkBufferUsageFlags ToVulkanBufferUsage(ResourceType type);

    VkImageUsageFlags ToVulkanImageUsage(ResourceType type);

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

    VkDescriptorType ToVulkanDescriptorType(ResourceType type);

    VkIndexType ToVulkanIndexType(IndexType type);

    VkAccessFlags ToVulkanAccessFlags(ResourceState state);

    VkImageLayout ToVulkanImageLayout(ResourceState state);

    VkPipelineStageFlags ToPipelineStageFlags(VkAccessFlags access_flags, QueueType queue_type);
}

