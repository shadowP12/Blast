#pragma once
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <volk.h>
#include <stdlib.h>

#ifdef VULKAN_DEBUG
#define VK_ASSERT(x)                                                \
	do                                                              \
	{                                                               \
		if (x != VK_SUCCESS)                                        \
		{                                                           \
			printf("Vulkan error at %s:%d.\n", __FILE__, __LINE__); \
			abort();                                                \
		}                                                           \
	} while (0)
#else
#define VK_ASSERT(x) ((void)0)
#endif