#pragma once
#include "VulkanDefine.h"

namespace blast {
    class VulkanDevice;

    class VulkanSampler : public GfxSampler {
    public:
        VulkanSampler(VulkanDevice*);

        virtual ~VulkanSampler();

    private:
        friend VulkanDevice;
        VulkanDevice* device = nullptr;
        VkSampler sampler{};
    };

    class VulkanBuffer : public GfxBuffer {
    public:
        VulkanBuffer(VulkanDevice*);

        virtual ~VulkanBuffer();

    private:
        friend VulkanDevice;
        VulkanDevice* device = nullptr;
        VkBuffer resource = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkBufferView srv = VK_NULL_HANDLE;
        int srv_index = -1;
        VkBufferView uav = VK_NULL_HANDLE;
        int uav_index = -1;
    };

    class VulkanTexture : public GfxTexture {
    public:
        VulkanTexture(VulkanDevice*);

        virtual ~VulkanTexture();

    private:
        friend VulkanDevice;
        VulkanDevice* device = nullptr;
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
        VulkanRenderPass(VulkanDevice*);

        virtual ~VulkanRenderPass();

    private:
        friend VulkanDevice;
        VulkanDevice* device = nullptr;
        VkRenderPass renderpass = VK_NULL_HANDLE;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkRenderPassBeginInfo begin_info = {};
        VkClearValue clear_colors[9] = {};
    };

    class VulkanSwapChain : public GfxSwapChain {
    public:
        VulkanSwapChain(VulkanDevice*);

        virtual ~VulkanSwapChain();

    private:
        friend class VulkanDevice;
        VulkanDevice* device = nullptr;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkFormat swapchain_image_format;
        VkExtent2D swapchain_extent;
        std::vector<VkImage> swapchain_images;
        std::vector<VkImageView> swapchain_image_views;
        std::vector<VkFramebuffer> swapchain_framebuffers;

        size_t renderpass_hash = 0;
        VkRenderPass renderpass = VK_NULL_HANDLE;

        VkSurfaceKHR surface = VK_NULL_HANDLE;

        uint32_t swapchain_image_index = 0;
        VkSemaphore swapchain_acquire_semaphore = VK_NULL_HANDLE;
        VkSemaphore swapchain_release_semaphore = VK_NULL_HANDLE;
    };

    class VulkanShader : public GfxShader {
    public:
        VulkanShader(VulkanDevice*);

        virtual ~VulkanShader();

    private:
        friend class VulkanDevice;
        VulkanDevice* device = nullptr;
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
        VulkanPipeline(VulkanDevice*);

        virtual ~VulkanPipeline();

    private:
        friend class VulkanDevice;
        VulkanDevice* device = nullptr;
        size_t hash = 0;
        VkPipeline pipeline = VK_NULL_HANDLE;
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
}