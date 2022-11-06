#pragma once
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "../GfxDefine.h"

#include <volk.h>
#include <stdlib.h>
#include <vector>

#define VK_ASSERT(x)                                                    \
	do                                                                  \
	{                                                                   \
		if (x != VK_SUCCESS)                                            \
		{                                                               \
			BLAST_LOGE("Vulkan error at %s:%d.\n", __FILE__, __LINE__); \
			abort();                                                    \
		}                                                               \
	} while (0)

// UniformBuffer偏移值
#define VULKAN_BINDING_SHIFT_B 0

// Texture偏移值
#define VULKAN_BINDING_SHIFT_T 1000

// UAV偏移值
#define VULKAN_BINDING_SHIFT_U 2000

// Sampler偏移值
#define VULKAN_BINDING_SHIFT_S 3000

namespace blast {
    class VulkanDevice;

    class VulkanSampler : public GfxSampler {
    public:
        VulkanSampler() = default;

        ~VulkanSampler() = default;

    private:
        friend VulkanDevice;
        VkSampler sampler;
    };

    class VulkanBuffer : public GfxBuffer {
    public:
        VulkanBuffer() = default;

        ~VulkanBuffer() = default;

    private:
        friend VulkanDevice;
        VkBuffer resource = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkBufferView srv = VK_NULL_HANDLE;
        int srv_index = -1;
        VkBufferView uav = VK_NULL_HANDLE;
        int uav_index = -1;
    };

    class VulkanTexture : public GfxTexture {
    public:
        VulkanTexture() = default;

        ~VulkanTexture() = default;

    private:
        friend VulkanDevice;
        VkImage resource = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView srv = VK_NULL_HANDLE;
        int srv_index = -1;
        VkImageView uav = VK_NULL_HANDLE;
        int uav_index = -1;
        VkImageView rtv = VK_NULL_HANDLE;
        VkImageView dsv = VK_NULL_HANDLE;
        std::vector<VkImageView> subresources_srv;
        std::vector<VkImageView> subresources_uav;
        std::vector<VkImageView> subresources_rtv;
        std::vector<VkImageView> subresources_dsv;
    };

    class VulkanRenderPass : public GfxRenderPass {
    public:
        VulkanRenderPass() = default;

        ~VulkanRenderPass() = default;

    private:
        friend VulkanDevice;
        VkRenderPass renderpass = VK_NULL_HANDLE;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkRenderPassBeginInfo begin_info = {};
        VkClearValue clear_colors[9] = {};
    };

    class VulkanSwapChain : public GfxSwapChain {
    public:
        VulkanSwapChain() = default;

        ~VulkanSwapChain() = default;

    private:
        friend class VulkanDevice;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkFormat swapchain_image_format;
        VkExtent2D swapchain_extent;
        std::vector<VkImage> swapchain_images;
        std::vector<VkImageView> swapchain_image_views;
        std::vector<VkFramebuffer> swapchain_framebuffers;

        size_t renderpass_hash;
        VkRenderPass renderpass;

        VkSurfaceKHR surface = VK_NULL_HANDLE;

        uint32_t swapchain_image_index = 0;
        VkSemaphore swapchain_acquire_semaphore = VK_NULL_HANDLE;
        VkSemaphore swapchain_release_semaphore = VK_NULL_HANDLE;
    };

    class VulkanShader : public GfxShader {
    public:
        VulkanShader() = default;

        ~VulkanShader() = default;

    private:
        friend class VulkanDevice;
        VkShaderModule shader_module = VK_NULL_HANDLE;
        VkPipeline pipeline_cs = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo stage_info = {};
        VkPipelineLayout pipeline_layout_cs = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
        VkPushConstantRange pushconstants = {};
        std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
        std::vector<VkImageViewType> image_view_types;
        size_t binding_hash = 0;
    };

    class VulkanPipeline : public GfxPipeline {
    public:
        VulkanPipeline() = default;

        ~VulkanPipeline() = default;

    private:
        friend class VulkanDevice;
        size_t hash = 0;
        VkPipeline pipeline;
        VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
        std::vector<VkImageViewType> image_view_types;
        VkPushConstantRange pushconstants = {};
        VkGraphicsPipelineCreateInfo pipeline_info = {};
        VkPipelineShaderStageCreateInfo shader_stages[SHADER_STAGE_COUNT] = {};
        VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        VkPipelineViewportStateCreateInfo viewport_state = {};
        VkPipelineDepthStencilStateCreateInfo depthstencil = {};
        VkPipelineTessellationStateCreateInfo tessellation_state = {};
        VkSampleMask samplemask = {};
    };

    class VulkanCommandBuffer : public GfxCommandBuffer {
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
}

