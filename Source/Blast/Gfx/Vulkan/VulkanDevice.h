#pragma once
#include "../GfxDevice.h"
#include "VulkanDefine.h"

#include <vector>
#include <mutex>
#include <deque>
#include <algorithm>
#include <unordered_map>

namespace blast {
    class VulkanDevice : public GfxDevice {
    public:
        VulkanDevice();

        ~VulkanDevice();

        uint64_t GetFrameCount() { return frame_count; }

        uint32_t GetBufferCount() { return BLAST_BUFFER_COUNT; }

        uint32_t GetBufferIndex() { return GetFrameCount() % BLAST_BUFFER_COUNT; }

        GfxBuffer* CreateBuffer(const GfxBufferDesc& desc) override;

        void DestroyBuffer(GfxBuffer*) override;

        GfxTexture* CreateTexture(const GfxTextureDesc& desc) override;

        void DestroyTexture(GfxTexture*) override;

        int32_t CreateSubresource(GfxBuffer*, SubResourceType, uint32_t, uint32_t) override;

        int32_t CreateSubresource(GfxTexture*, SubResourceType, uint32_t, uint32_t, uint32_t, uint32_t) override;

        GfxSampler* CreateSampler(const GfxSamplerDesc& desc) override;

        void DestroySampler(GfxSampler*) override;

        GfxSwapChain* CreateSwapChain(const GfxSwapChainDesc& desc, GfxSwapChain* old_swapchain = nullptr) override;

        void DestroySwapChain(GfxSwapChain*) override;

        GfxRenderPass* CreateRenderPass(const GfxRenderPassDesc& desc) override;

        void DestroyRenderPass(GfxRenderPass*) override;

        GfxShader* CreateShader(const GfxShaderDesc& desc) override;

        void DestroyShader(GfxShader*) override;

        GfxPipeline* CreatePipeline(const GfxPipelineDesc& desc) override;

        void DestroyPipeline(GfxPipeline*) override;

        GfxCommandBuffer* RequestCommandBuffer(QueueType type) override;

        void SubmitAllCommandBuffer() override;

        void WaitCommandBuffer(GfxCommandBuffer* cmd, GfxCommandBuffer* wait_for) override;

        void RenderPassBegin(GfxCommandBuffer* cmd, GfxSwapChain* swapchain) override;

        void RenderPassBegin(GfxCommandBuffer* cmd, GfxRenderPass* renderpass) override;

        void RenderPassEnd(GfxCommandBuffer* cmd) override;

        void BindScissorRects(GfxCommandBuffer* cmd, uint32_t num_rects, Rect* rects) override;

        void BindViewports(GfxCommandBuffer* cmd, uint32_t num_viewports, Viewport* viewports) override;

        void BindResource(GfxCommandBuffer* cmd, GfxResource* resource, uint32_t slot, int32_t subresource = -1) override;

        void BindResources(GfxCommandBuffer* cmd, GfxResource** resources, uint32_t slot, uint32_t count) override;

        void BindUAV(GfxCommandBuffer* cmd, GfxResource* resource, uint32_t slot, int32_t subresource = -1) override;

        void BindUAVs(GfxCommandBuffer* cmd, GfxResource** resources, uint32_t slot, uint32_t count) override;

        void BindSampler(GfxCommandBuffer* cmd, GfxSampler* sampler, uint32_t slot) override;

        void BindConstantBuffer(GfxCommandBuffer* cmd, GfxBuffer* buffer, uint32_t slot, uint64_t offset = 0ull) override;

        void BindVertexBuffers(GfxCommandBuffer* cmd, GfxBuffer** vertex_buffers, uint32_t slot, uint32_t count, uint64_t* offsets) override;

        void BindIndexBuffer(GfxCommandBuffer* cmd, GfxBuffer* index_buffer, IndexType type, uint64_t offset) override;

        void BindStencilRef(GfxCommandBuffer* cmd, uint32_t value) override;

        void BindBlendFactor(GfxCommandBuffer* cmd, float r, float g, float b, float a) override;

        void BindPipeline(GfxCommandBuffer* cmd, GfxPipeline* pipeline) override;

        void BindComputeShader(GfxCommandBuffer* cmd, GfxShader* cs) override;

        void PushConstants(GfxCommandBuffer* cmd, const void* data, uint32_t size) override;

        void Draw(GfxCommandBuffer* cmd, uint32_t vertex_count, uint32_t vertex_offset) override;

        void DrawIndexed(GfxCommandBuffer* cmd, uint32_t index_count, uint32_t index_offset, int32_t vertex_offset) override;

        void DrawInstanced(GfxCommandBuffer* cmd, uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset) override;

        void DrawIndexedInstanced(GfxCommandBuffer* cmd, uint32_t index_count, uint32_t instance_count, uint32_t index_offset, int32_t vertex_offset, uint32_t instance_offset) override;

        void Dispatch(GfxCommandBuffer* cmd, uint32_t thread_group_x, uint32_t thread_group_y, uint32_t thread_group_z) override;

        void BufferCopy(GfxCommandBuffer* cmd, const GfxBufferCopyRange& range) override;

        void ImageCopy(GfxCommandBuffer* cmd, const GfxImageCopyRange& range) override;

        void BufferImageCopy(GfxCommandBuffer* cmd, const GfxBufferImageCopyRange& range) override;

        void UpdateBuffer(GfxCommandBuffer* cmd, GfxBuffer* buffer, const void* data, uint64_t size = 0, uint64_t offset = 0) override;

        void UpdateTexture(GfxCommandBuffer* cmd, GfxTexture* texture, const void* data, uint32_t layer = 0, uint32_t level = 0) override;

        void SetBarrier(GfxCommandBuffer* cmd, uint32_t num_buffer_barriers, GfxBufferBarrier* buffer_barriers,
                                uint32_t num_texture_barriers, GfxTextureBarrier* texture_barriers) override;

    protected:
        uint32_t FindMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties);

        void PipelineStateValidate(uint32_t cmd);

        void PreDraw(uint32_t cmd);

        void PreDispatch(uint32_t cmd);

    protected:
        struct Queue {
            void Submit(VkFence fence);

            VkQueue queue = VK_NULL_HANDLE;
            VkSemaphore semaphore = VK_NULL_HANDLE;
            std::vector<VkSwapchainKHR> submit_swapchains;
            std::vector<uint32_t> submit_swapchain_image_indices;
            std::vector<VkPipelineStageFlags> submit_wait_stages;
            std::vector<VkSemaphore> submit_wait_semaphores;
            std::vector<uint64_t> submit_wait_values;
            std::vector<VkSemaphore> submit_signal_semaphores;
            std::vector<uint64_t> submit_signal_values;
            std::vector<VkCommandBuffer> submit_cmds;
        } queues[BLAST_QUEUE_COUNT];

        // 一帧内可能会有多个线程访问InitCommandBuffer,所以需要加锁
        mutable std::mutex init_locker;
        mutable bool submit_inits = false;

        struct StageBuffer {
            struct Allocation {
                GfxBuffer* buffer;
                uint64_t offset = 0;
            };

            void Init(VulkanDevice* device);

            Allocation Allocate(uint32_t size);

            void Destroy();

            VulkanDevice* device = nullptr;
            uint64_t size = 0;
            uint64_t offset = 0;
            uint64_t frame_index = 0;
            GfxBuffer* buffer = nullptr;
        };

        struct Frame {
            struct DescriptorPool {
                void Init(VulkanDevice* device);

                void Destroy();

                void Reset();

                VulkanDevice* device = nullptr;
                VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
                uint32_t pool_size = 256;
            } descriptor_pools[BLAST_CMD_COUNT];

            StageBuffer* stage_buffers[BLAST_CMD_COUNT];
            VkFence fence[BLAST_QUEUE_COUNT] = {};
            VkCommandPool command_pools[BLAST_CMD_COUNT][BLAST_QUEUE_COUNT] = {};
            VkCommandBuffer command_buffers[BLAST_CMD_COUNT][BLAST_QUEUE_COUNT] = {};
            VkCommandPool init_command_pool = VK_NULL_HANDLE;
            VkCommandBuffer init_command_buffer = VK_NULL_HANDLE;
        } frames[BLAST_BUFFER_COUNT];

        Frame& GetFrameResources() { return frames[GetBufferIndex()]; }

        struct CommandListMetadata {
            QueueType queue = {};
            std::vector<uint32_t> waits;
        } cmd_meta[BLAST_CMD_COUNT];

        VkCommandBuffer GetCommandBuffer(uint32_t cmd) {
            return GetFrameResources().command_buffers[cmd][cmd_meta[cmd].queue];
        }

        struct DescriptorBinder {
            GfxBindingTable table;
            VulkanDevice* device;

            std::vector<VkWriteDescriptorSet> descriptor_writes;
            std::vector<VkDescriptorBufferInfo> buffer_infos;
            std::vector<VkDescriptorImageInfo> image_infos;
            std::vector<VkBufferView> texel_buffer_views;
            std::vector<VkWriteDescriptorSetAccelerationStructureKHR> acceleration_structure_views;
            bool dirty = false;

            void Init(VulkanDevice* device);

            void Reset();

            void Flush(bool graphics, uint32_t cmd);
        };
        DescriptorBinder binders[BLAST_CMD_COUNT];

        // 负责Copy命令缓存的分配以及管理
        struct CopyCommandBufferPool {
            struct CopyCommandBuffer {
                VkCommandPool command_pool = VK_NULL_HANDLE;
                VkCommandBuffer command_buffer = VK_NULL_HANDLE;
                uint32_t id = 0;
                uint64_t target = 0;
                StageBuffer* stage_buffer = nullptr;
            };

            VulkanDevice* device = nullptr;
            VkSemaphore semaphore = VK_NULL_HANDLE;
            uint32_t next_id = BLAST_CMD_COUNT + 1;
            uint64_t fence_value = 0;
            std::vector<CopyCommandBuffer> freelist;
            std::vector<CopyCommandBuffer> worklist;
            std::vector<CopyCommandBuffer> submitlist;
            std::unordered_map<uint32_t, CopyCommandBuffer> total_cmds;

            void Init(VulkanDevice* device);

            void Destroy();

            uint32_t Allocate();

            uint64_t Submit();
        } copy_pool;

        CopyCommandBufferPool::CopyCommandBuffer GetCopyCommandBuffer(uint32_t cmd) {
            return copy_pool.total_cmds[cmd];
        }

        struct ResourceManager {
            VkDevice device = VK_NULL_HANDLE;
            VkInstance instance = VK_NULL_HANDLE;
            uint64_t frame_count = 0;
            std::mutex destroy_locker;

            std::deque<std::pair<std::pair<VkImage, VkDeviceMemory>, uint64_t>> destroyer_images;
            std::deque<std::pair<VkImageView, uint64_t>> destroyer_imageviews;
            std::deque<std::pair<std::pair<VkBuffer, VkDeviceMemory>, uint64_t>> destroyer_buffers;
            std::deque<std::pair<VkBufferView, uint64_t>> destroyer_bufferviews;
            std::deque<std::pair<VkAccelerationStructureKHR, uint64_t>> destroyer_bvhs;
            std::deque<std::pair<VkSampler, uint64_t>> destroyer_samplers;
            std::deque<std::pair<VkDescriptorPool, uint64_t>> destroyer_descriptor_pools;
            std::deque<std::pair<VkDescriptorSetLayout, uint64_t>> destroyer_descriptor_set_layouts;
            std::deque<std::pair<VkDescriptorUpdateTemplate, uint64_t>> destroyer_descriptor_update_templates;
            std::deque<std::pair<VkShaderModule, uint64_t>> destroyer_shadermodules;
            std::deque<std::pair<VkPipelineLayout, uint64_t>> destroyer_pipeline_layouts;
            std::deque<std::pair<VkPipeline, uint64_t>> destroyer_pipelines;
            std::deque<std::pair<VkRenderPass, uint64_t>> destroyer_renderpasses;
            std::deque<std::pair<VkFramebuffer, uint64_t>> destroyer_framebuffers;
            std::deque<std::pair<VkQueryPool, uint64_t>> destroyer_querypools;

            void Update(uint64_t current_frame_count, uint32_t buffer_count);

            void Clear();
        } resource_manager;

        std::vector<GfxCommandBuffer*> copy_cmds;
        std::vector<GfxCommandBuffer*> work_cmds;

        bool dirty_pipeline[BLAST_CMD_COUNT] = {};
        GfxPipeline* active_pipeline[BLAST_CMD_COUNT] = {};
        GfxShader* active_cs[BLAST_CMD_COUNT] = {};

        std::vector<GfxSwapChain*> active_swapchains[BLAST_CMD_COUNT];

        struct DeferredPushConstantData {
            uint8_t data[128];
            uint32_t size;
        };
        DeferredPushConstantData pushconstants[BLAST_CMD_COUNT] = {};

        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT debug_report_callback = VK_NULL_HANDLE;
        VkPhysicalDevice phy_device = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties phy_device_properties;
        VkPhysicalDeviceFeatures phy_device_features;
        VkPhysicalDeviceMemoryProperties phy_device_memory_properties;
        VkDevice device = VK_NULL_HANDLE;
        VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
        VkQueue graphics_queue = VK_NULL_HANDLE;
        VkQueue compute_queue = VK_NULL_HANDLE;
        VkQueue copy_queue = VK_NULL_HANDLE;
        uint32_t graphics_family = -1;
        uint32_t compute_family = -1;
        uint32_t copy_family = -1;
        std::vector<VkQueueFamilyProperties> queue_family_properties;
    };
}