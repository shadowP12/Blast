#pragma once
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <volk.h>
#include <stdlib.h>
#include "../GfxDefine.h"
#include "../../Utility/Logging.h"

#ifdef VULKAN_DEBUG
#define VK_ASSERT(x)                                                    \
	do                                                                  \
	{                                                                   \
		if (x != VK_SUCCESS)                                            \
		{                                                               \
			BLAST_LOGE("Vulkan error at %s:%d.\n", __FILE__, __LINE__); \
			abort();                                                    \
		}                                                               \
	} while (0)
#else
#define VK_ASSERT(x) ((void)0)
#endif

namespace Blast {
    VkSampleCountFlagBits toVulkanSampleCount(SampleCount sampleCount);

    VkFormat toVulkanFormat(Format format);

    VkImageAspectFlags toVulkanAspectMask(Format format);

    VkBufferUsageFlags toVulkanBufferUsage(ResourceType type);

    VkImageUsageFlags toVulkanImageUsage(ResourceType type);
}

