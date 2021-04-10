#pragma once
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <volk.h>
#include <stdlib.h>
#include "../GfxDefine.h"
#include "../../Utility/Logging.h"

#define VK_ASSERT(x)                                                    \
	do                                                                  \
	{                                                                   \
		if (x != VK_SUCCESS)                                            \
		{                                                               \
			BLAST_LOGE("Vulkan error at %s:%d.\n", __FILE__, __LINE__); \
			abort();                                                    \
		}                                                               \
	} while (0)

namespace Blast {
    VkSampleCountFlagBits toVulkanSampleCount(SampleCount sampleCount);

    VkFormat toVulkanFormat(Format format);

    Format toGfxFormat(VkFormat format);

    VkImageAspectFlags toVulkanAspectMask(Format format);

    VkBufferUsageFlags toVulkanBufferUsage(ResourceType type);

    VkImageUsageFlags toVulkanImageUsage(ResourceType type);

    VkAttachmentLoadOp toVulkanLoadOp(LoadAction op);

    VkBlendOp toVulkanBlendOp(BlendOp op);

    VkBlendFactor toVulkanBlendFactor(BlendConstant factor);

    VkStencilOp toVulkanStencilOp(StencilOp op);

    VkCompareOp toVulkanCompareOp(CompareMode op);

    VkPolygonMode toVulkanFillMode(FillMode mode);

    VkCullModeFlagBits toVulkanCullMode(CullMode mode);

    VkFrontFace toVulkanFrontFace(FrontFace frontFace);

    VkFilter toVulkanFilter(FilterType filter);

    VkSamplerMipmapMode toVulkanMipmapMode(MipmapMode mode);

    VkSamplerAddressMode toVulkanAddressMode(AddressMode mode);

    VkShaderStageFlags toVulkanShaderStages(ShaderStage stages);

    VkDescriptorType toVulkanDescriptorType(ResourceType type);

    VkIndexType toVulkanIndexType(IndexType type);

    VkAccessFlags toVulkanAccessFlags(ResourceState state);

    VkImageLayout toVulkanImageLayout(ResourceState state);

    VkPipelineStageFlags toPipelineStageFlags(VkAccessFlags accessFlags, QueueType queueType);
}

