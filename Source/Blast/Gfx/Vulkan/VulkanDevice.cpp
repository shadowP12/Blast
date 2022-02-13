#include "VulkanDevice.h"
#include "spirv_reflect.h"

namespace blast {
    static bool IsLayerSupported(const char *required, const std::vector<VkLayerProperties> &available) {
        for (const VkLayerProperties &availableLayer : available) {
            if (strcmp(availableLayer.layerName, required) == 0) {
                return true;
            }
        }
        return false;
    }

    static bool IsExtensionSupported(const char *required, const std::vector<VkExtensionProperties> &available) {
        for (const VkExtensionProperties &available_extension : available) {
            if (strcmp(available_extension.extensionName, required) == 0) {
                return true;
            }
        }
        return false;
    }

#if VULKAN_DEBUG
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                               VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                               const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                               void* user_data) {
        if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            BLAST_LOGW("[Vulkan]: Validation Error %s: %s\n", callback_data->pMessageIdName, callback_data->pMessage);
        } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            BLAST_LOGE("[Vulkan]: Validation Warning %s: %s\n", callback_data->pMessageIdName, callback_data->pMessage);
        }
        return VK_FALSE;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags,
                                                       VkDebugReportObjectTypeEXT type,
                                                       uint64_t object,
                                                       size_t location,
                                                       int32_t message_code,
                                                       const char* layer_prefix,
                                                       const char* message,
                                                       void* user_data) {
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            BLAST_LOGE("[Vulkan]: Validation Error: %s: %s\n", layer_prefix, message);
        } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
            BLAST_LOGW("[Vulkan]: Validation Warning: %s: %s\n", layer_prefix, message);
        } else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
            BLAST_LOGI("[Vulkan]: Validation PerfWarning: %s: %s\n", layer_prefix, message);
        } else {
            BLAST_LOGI("[Vulkan]: Validation Info: %s: %s\n", layer_prefix, message);
        }
        return VK_FALSE;
    }
#endif

    void VulkanDevice::StageBuffer::Init(VulkanDevice* device) {
        this->device = device;
    }

    VulkanDevice::StageBuffer::Allocation VulkanDevice::StageBuffer::Allocate(uint32_t data_size) {
        if (device->frame_count != frame_index) {
            offset = 0;
            frame_index = device->frame_count;
        }

        const uint64_t free_space = size - offset;
        if (data_size > free_space || buffer == nullptr) {
            if (buffer) {
                device->DestroyBuffer(buffer);
            }

            size = AlignTo((size + data_size) * 2, 8);
            offset = 0;

            GfxBufferDesc desc;
            desc.size = size;
            desc.mem_usage = MEMORY_USAGE_CPU_TO_GPU;
            desc.res_usage = RESOURCE_USAGE_RW_BUFFER;
            buffer = device->CreateBuffer(desc);
        }

        Allocation allocation;
        allocation.buffer = buffer;
        allocation.offset = offset;

        offset += AlignTo(data_size, 8);

        return allocation;
    }

    void VulkanDevice::StageBuffer::Destroy() {
        if (buffer) {
            device->DestroyBuffer(buffer);
        }
    }

    void VulkanDevice::Frame::DescriptorPool::Init(VulkanDevice* device) {
        this->device = device;

        // Create descriptor pool:
        VkDescriptorPoolSize pool_sizes[9] = {};
        uint32_t count = 0;

        pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount = BLAST_CBV_COUNT * pool_size;
        count++;

        pool_sizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        pool_sizes[1].descriptorCount = BLAST_SRV_COUNT * pool_size;
        count++;

        pool_sizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        pool_sizes[2].descriptorCount = BLAST_SRV_COUNT * pool_size;
        count++;

        pool_sizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        pool_sizes[3].descriptorCount = BLAST_SRV_COUNT * pool_size;
        count++;

        pool_sizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        pool_sizes[4].descriptorCount = BLAST_UAV_COUNT * pool_size;
        count++;

        pool_sizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        pool_sizes[5].descriptorCount = BLAST_UAV_COUNT * pool_size;
        count++;

        pool_sizes[6].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        pool_sizes[6].descriptorCount = BLAST_UAV_COUNT * pool_size;
        count++;

        pool_sizes[7].type = VK_DESCRIPTOR_TYPE_SAMPLER;
        pool_sizes[7].descriptorCount = BLAST_SAMPLER_COUNT * pool_size;
        count++;

        pool_sizes[8].type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        pool_sizes[8].descriptorCount = BLAST_SRV_COUNT * pool_size;
        count++;

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = count;
        pool_info.pPoolSizes = pool_sizes;
        pool_info.maxSets = pool_size;
        VK_ASSERT(vkCreateDescriptorPool(device->device, &pool_info, nullptr, &descriptor_pool));
    }

    void VulkanDevice::Frame::DescriptorPool::Destroy() {
        if (descriptor_pool != VK_NULL_HANDLE) {
            device->resource_manager.destroy_locker.lock();
            device->resource_manager.destroyer_descriptor_pools.push_back(std::make_pair(descriptor_pool, device->frame_count));
            descriptor_pool = VK_NULL_HANDLE;
            device->resource_manager.destroy_locker.unlock();
        }
    }
    void VulkanDevice::Frame::DescriptorPool::Reset() {
        if (descriptor_pool != VK_NULL_HANDLE) {
            VK_ASSERT(vkResetDescriptorPool(device->device, descriptor_pool, 0));
        }
    }

    void VulkanDevice::DescriptorBinder::Init(VulkanDevice* device) {
        this->device = device;
        descriptor_writes.reserve(128);
        buffer_infos.reserve(128);
        image_infos.reserve(128);
        texel_buffer_views.reserve(128);
        acceleration_structure_views.reserve(128);
    }

    void VulkanDevice::DescriptorBinder::Reset() {
        table = {};
        dirty = true;
    }

    void VulkanDevice::DescriptorBinder::Flush(bool graphics, uint32_t cmd) {
        if (!dirty)
            return;
        dirty = false;

        auto& binder_pool = device->GetFrameResources().descriptor_pools[cmd];
        auto internal_pso = graphics ? (VulkanPipeline*)device->active_pipeline[cmd] : nullptr;
        auto internal_cs = graphics ? nullptr : (VulkanShader*)device->active_cs[cmd];

        VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
        if (graphics) {
            pipeline_layout = internal_pso->pipeline_layout;
            descriptor_set_layout = internal_pso->descriptor_set_layout;
        }
        else {
            pipeline_layout = internal_cs->pipeline_layout_cs;
            descriptor_set_layout = internal_cs->descriptor_set_layout;
        }

        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = binder_pool.descriptor_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &descriptor_set_layout;

        VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
        VkResult res = vkAllocateDescriptorSets(device->device, &alloc_info, &descriptor_set);
        while (res == VK_ERROR_OUT_OF_POOL_MEMORY) {
            binder_pool.pool_size *= 2;
            binder_pool.Destroy();
            binder_pool.Init(device);
            alloc_info.descriptorPool = binder_pool.descriptor_pool;
            res = vkAllocateDescriptorSets(device->device, &alloc_info, &descriptor_set);
        }
        assert(res == VK_SUCCESS);

        descriptor_writes.clear();
        buffer_infos.clear();
        image_infos.clear();
        texel_buffer_views.clear();
        acceleration_structure_views.clear();

        const auto& layout_bindings = graphics ? internal_pso->layout_bindings : internal_cs->layout_bindings;
        const auto& image_view_types = graphics ? internal_pso->image_view_types : internal_cs->image_view_types;

        uint32_t i = 0;
        for (auto& x : layout_bindings) {
            if (x.pImmutableSamplers != nullptr) {
                i++;
                continue;
            }

            VkImageViewType viewtype = image_view_types[i++];

            for (uint32_t descriptor_index = 0; descriptor_index < x.descriptorCount; ++descriptor_index) {
                uint32_t unrolled_binding = x.binding + descriptor_index;

                descriptor_writes.emplace_back();
                auto& write = descriptor_writes.back();
                write = {};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = descriptor_set;
                write.dstArrayElement = descriptor_index;
                write.descriptorType = x.descriptorType;
                write.dstBinding = x.binding;
                write.descriptorCount = 1;

                switch (x.descriptorType) {
                    case VK_DESCRIPTOR_TYPE_SAMPLER: {
                        image_infos.emplace_back();
                        write.pImageInfo = &image_infos.back();
                        image_infos.back() = {};

                        const uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_S;
                        const GfxSampler* sampler = table.sam[original_binding];
                        if (sampler != nullptr) {
                            image_infos.back().sampler = ((VulkanSampler*)sampler)->sampler;
                        }
                    }
                        break;

                    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: {
                        image_infos.emplace_back();
                        write.pImageInfo = &image_infos.back();
                        image_infos.back() = {};

                        uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_T;
                        GfxResource* resource = table.srv[original_binding];
                        if (resource != nullptr) {
                            int32_t subresource = table.srv_index[original_binding];
                            VulkanTexture* internal_texture = (VulkanTexture*)resource;
                            if (subresource >= 0) {
                                image_infos.back().imageView = internal_texture->subresources_srv[subresource];
                            } else {
                                image_infos.back().imageView = internal_texture->srv;
                            }

                            image_infos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        }
                    }
                        break;

                    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: {
                        image_infos.emplace_back();
                        write.pImageInfo = &image_infos.back();
                        image_infos.back() = {};
                        image_infos.back().imageLayout = VK_IMAGE_LAYOUT_GENERAL;

                        uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_U;
                        GfxResource* resource = table.uav[original_binding];
                        if (resource != nullptr) {
                            int32_t subresource = table.uav_index[original_binding];
                            VulkanTexture* internal_texture = (VulkanTexture*)resource;
                            if (subresource >= 0) {
                                image_infos.back().imageView = internal_texture->subresources_uav[subresource];
                            } else {
                                image_infos.back().imageView = internal_texture->uav;
                            }
                        }
                    }
                        break;

                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
                        buffer_infos.emplace_back();
                        write.pBufferInfo = &buffer_infos.back();
                        buffer_infos.back() = {};

                        uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_B;
                        GfxBuffer* buffer = table.cbv[original_binding];
                        uint64_t offset = table.cbv_offset[original_binding];
                        uint64_t size = table.cbv_size[original_binding];

                        if (buffer != nullptr) {
                            VulkanBuffer* internal_buffer = (VulkanBuffer*)buffer;
                            buffer_infos.back().buffer = internal_buffer->resource;
                            buffer_infos.back().offset = offset;
                            buffer_infos.back().range = size;
                        }
                    }
                        break;

                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: {
                        buffer_infos.emplace_back();
                        write.pBufferInfo = &buffer_infos.back();
                        buffer_infos.back() = {};

                        if (x.binding < VULKAN_BINDING_SHIFT_U) {
                            // SRV
                            uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_T;
                            GfxResource* resource = table.srv[original_binding];
                            if (resource != nullptr) {
                                int32_t subresource = table.srv_index[original_binding];
                                VulkanBuffer* internal_buffer = (VulkanBuffer*)resource;
                                buffer_infos.back().buffer = internal_buffer->resource;
                                buffer_infos.back().range = VK_WHOLE_SIZE;
                            }
                        }
                        else {
                            // UAV
                            uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_U;
                            GfxResource* resource = table.uav[original_binding];
                            if (resource != nullptr) {
                                int32_t subresource = table.uav_index[original_binding];
                                VulkanBuffer* internal_buffer = (VulkanBuffer*)resource;
                                buffer_infos.back().buffer = internal_buffer->resource;
                                buffer_infos.back().range = VK_WHOLE_SIZE;
                            }
                        }
                    }
                        break;

                    case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR: {
                        acceleration_structure_views.emplace_back();
                        write.pNext = &acceleration_structure_views.back();
                        acceleration_structure_views.back() = {};
                        acceleration_structure_views.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
                        acceleration_structure_views.back().accelerationStructureCount = 1;

                        uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_T;
                        GfxResource* resource = table.srv[original_binding];
                        if (resource != nullptr) {
                            assert(0);
                        }
                    }
                        break;

                }
            }
        }

        vkUpdateDescriptorSets(
                device->device,
                (uint32_t)descriptor_writes.size(),
                descriptor_writes.data(),
                0,
                nullptr
        );

        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        if (!graphics) {
            bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

            if (device->active_cs[cmd]->stage == SHADER_STAGE_RAYTRACING) {
                bindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
            }
        }

        vkCmdBindDescriptorSets(
                device->GetCommandBuffer(cmd),
                bindPoint,
                pipeline_layout,
                0,
                1,
                &descriptor_set,
                0,
                nullptr
        );
    }

    void VulkanDevice::Queue::Submit(VkFence fence) {
        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = (uint32_t)submit_cmds.size();
        submit_info.pCommandBuffers = submit_cmds.data();

        submit_info.waitSemaphoreCount = (uint32_t)submit_wait_semaphores.size();
        submit_info.pWaitSemaphores = submit_wait_semaphores.data();
        submit_info.pWaitDstStageMask = submit_wait_stages.data();

        submit_info.signalSemaphoreCount = (uint32_t)submit_signal_semaphores.size();
        submit_info.pSignalSemaphores = submit_signal_semaphores.data();

        VkTimelineSemaphoreSubmitInfo timeline_info = {};
        timeline_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
        timeline_info.pNext = nullptr;
        timeline_info.waitSemaphoreValueCount = (uint32_t)submit_wait_values.size();
        timeline_info.pWaitSemaphoreValues = submit_wait_values.data();
        timeline_info.signalSemaphoreValueCount = (uint32_t)submit_signal_values.size();
        timeline_info.pSignalSemaphoreValues = submit_signal_values.data();

        submit_info.pNext = &timeline_info;

        VK_ASSERT(vkQueueSubmit(queue, 1, &submit_info, fence));

        if (!submit_swapchains.empty()) {
            VkPresentInfoKHR present_info = {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = (uint32_t)submit_signal_semaphores.size();
            present_info.pWaitSemaphores = submit_signal_semaphores.data();
            present_info.swapchainCount = (uint32_t)submit_swapchains.size();
            present_info.pSwapchains = submit_swapchains.data();
            present_info.pImageIndices = submit_swapchain_image_indices.data();
            VK_ASSERT(vkQueuePresentKHR(queue, &present_info));
        }

        submit_swapchains.clear();
        submit_swapchain_image_indices.clear();
        submit_wait_stages.clear();
        submit_wait_semaphores.clear();
        submit_wait_values.clear();
        submit_signal_semaphores.clear();
        submit_signal_values.clear();
        submit_cmds.clear();
    }

    void VulkanDevice::CopyCommandBufferPool::Init(VulkanDevice* device) {
        this->device = device;

        VkSemaphoreTypeCreateInfo timeline_info = {};
        timeline_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        timeline_info.pNext = nullptr;
        timeline_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        timeline_info.initialValue = 0;

        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphore_info.pNext = &timeline_info;
        semaphore_info.flags = 0;

        VK_ASSERT(vkCreateSemaphore(device->device, &semaphore_info, nullptr, &semaphore));
    }

    void VulkanDevice::CopyCommandBufferPool::Destroy() {
        vkQueueWaitIdle(device->copy_queue);
        for (auto& x : total_cmds) {
            if (x.second.stage_buffer) {
                x.second.stage_buffer->Destroy();
                BLAST_SAFE_DELETE(x.second.stage_buffer);
            }

            vkDestroyCommandPool(device->device, x.second.command_pool, nullptr);
        }
        vkDestroySemaphore(device->device, semaphore, nullptr);
    }

    uint32_t VulkanDevice::CopyCommandBufferPool::Allocate() {
        if (freelist.empty()) {
            CopyCommandBuffer cmd;
            cmd.id = next_id++;

            cmd.stage_buffer = new StageBuffer();
            cmd.stage_buffer->Init(device);

            VkCommandPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            pool_info.queueFamilyIndex = device->copy_family;
            pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            VK_ASSERT(vkCreateCommandPool(device->device, &pool_info, nullptr, &cmd.command_pool));

            VkCommandBufferAllocateInfo command_buffer_info = {};
            command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            command_buffer_info.commandBufferCount = 1;
            command_buffer_info.commandPool = cmd.command_pool;
            command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            VK_ASSERT(vkAllocateCommandBuffers(device->device, &command_buffer_info, &cmd.command_buffer));

            freelist.push_back(cmd);
            total_cmds[cmd.id] = cmd;
        }

        CopyCommandBuffer cmd = freelist.back();
        freelist.pop_back();
        submitlist.push_back(cmd);

        vkResetCommandPool(device->device, cmd.command_pool, 0);

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = nullptr;
        vkBeginCommandBuffer(cmd.command_buffer, &begin_info);

        return cmd.id;
    }

    uint64_t VulkanDevice::CopyCommandBufferPool::Submit() {
        uint64_t submit_wait = 0;
        std::vector<VkCommandBuffer> submit_cmds;
        for (uint32_t i = 0; i < submitlist.size(); ++i) {
            vkEndCommandBuffer(submitlist[i].command_buffer);

            submitlist[i].target = ++fence_value;
            worklist.push_back(submitlist[i]);
            submit_cmds.push_back(submitlist[i].command_buffer);
            submit_wait = std::max(submit_wait, submitlist[i].target);
        }
        submitlist.clear();

        if (!submit_cmds.empty())
        {
            VkSubmitInfo submit_info = {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info.commandBufferCount = (uint32_t)submit_cmds.size();
            submit_info.pCommandBuffers = submit_cmds.data();
            submit_info.pSignalSemaphores = &semaphore;
            submit_info.signalSemaphoreCount = 1;

            VkTimelineSemaphoreSubmitInfo timeline_info = {};
            timeline_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
            timeline_info.pNext = nullptr;
            timeline_info.waitSemaphoreValueCount = 0;
            timeline_info.pWaitSemaphoreValues = nullptr;
            timeline_info.signalSemaphoreValueCount = 1;
            timeline_info.pSignalSemaphoreValues = &submit_wait;

            submit_info.pNext = &timeline_info;

            VK_ASSERT(vkQueueSubmit(device->copy_queue, 1, &submit_info, VK_NULL_HANDLE));
        }

        uint64_t completed_fence_value;
        VK_ASSERT(vkGetSemaphoreCounterValue(device->device, semaphore, &completed_fence_value));
        for (uint32_t i = 0; i < worklist.size(); ++i) {
            if (worklist[i].target <= completed_fence_value) {
                freelist.push_back(worklist[i]);
                worklist[i] = worklist.back();
                worklist.pop_back();
                i--;
            }
        }

        return submit_wait;
    }

    void VulkanDevice::ResourceManager::Update(uint64_t current_frame_count, uint32_t buffer_count) {
        destroy_locker.lock();
        frame_count = current_frame_count;
        while (!destroyer_images.empty())
        {
            if (destroyer_images.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_images.front();
                destroyer_images.pop_front();
                vkDestroyImage(device, item.first.first, nullptr);
                vkFreeMemory(device, item.first.second, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_imageviews.empty())
        {
            if (destroyer_imageviews.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_imageviews.front();
                destroyer_imageviews.pop_front();
                vkDestroyImageView(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_buffers.empty())
        {
            if (destroyer_buffers.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_buffers.front();
                destroyer_buffers.pop_front();
                vkDestroyBuffer(device, item.first.first, nullptr);
                vkFreeMemory(device, item.first.second, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_bufferviews.empty())
        {
            if (destroyer_bufferviews.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_bufferviews.front();
                destroyer_bufferviews.pop_front();
                vkDestroyBufferView(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_bvhs.empty())
        {
            if (destroyer_bvhs.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_bvhs.front();
                destroyer_bvhs.pop_front();
                vkDestroyAccelerationStructureKHR(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_samplers.empty())
        {
            if (destroyer_samplers.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_samplers.front();
                destroyer_samplers.pop_front();
                vkDestroySampler(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_descriptor_pools.empty())
        {
            if (destroyer_descriptor_pools.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_descriptor_pools.front();
                destroyer_descriptor_pools.pop_front();
                vkDestroyDescriptorPool(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_descriptor_set_layouts.empty()) {
            if (destroyer_descriptor_set_layouts.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_descriptor_set_layouts.front();
                destroyer_descriptor_set_layouts.pop_front();
                vkDestroyDescriptorSetLayout(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_descriptor_update_templates.empty())
        {
            if (destroyer_descriptor_update_templates.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_descriptor_update_templates.front();
                destroyer_descriptor_update_templates.pop_front();
                vkDestroyDescriptorUpdateTemplate(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_shadermodules.empty())
        {
            if (destroyer_shadermodules.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_shadermodules.front();
                destroyer_shadermodules.pop_front();
                vkDestroyShaderModule(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_pipeline_layouts.empty()) {
            if (destroyer_pipeline_layouts.front().second + buffer_count < frame_count) {
                auto item = destroyer_pipeline_layouts.front();
                destroyer_pipeline_layouts.pop_front();
                vkDestroyPipelineLayout(device, item.first, nullptr);
            } else {
                break;
            }
        }
        while (!destroyer_pipelines.empty())
        {
            if (destroyer_pipelines.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_pipelines.front();
                destroyer_pipelines.pop_front();
                vkDestroyPipeline(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_renderpasses.empty())
        {
            if (destroyer_renderpasses.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_renderpasses.front();
                destroyer_renderpasses.pop_front();
                vkDestroyRenderPass(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_framebuffers.empty())
        {
            if (destroyer_framebuffers.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_framebuffers.front();
                destroyer_framebuffers.pop_front();
                vkDestroyFramebuffer(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }
        while (!destroyer_querypools.empty())
        {
            if (destroyer_querypools.front().second + buffer_count < frame_count)
            {
                auto item = destroyer_querypools.front();
                destroyer_querypools.pop_front();
                vkDestroyQueryPool(device, item.first, nullptr);
            }
            else
            {
                break;
            }
        }

        destroy_locker.unlock();
    }

    void VulkanDevice::ResourceManager::Clear() {
        Update(~0, 0);
    }

    VulkanDevice::VulkanDevice()
    : GfxDevice() {
        if (volkInitialize() != VK_SUCCESS) {
            return;
        }
        uint32_t num_instance_available_layers;
        VK_ASSERT(vkEnumerateInstanceLayerProperties(&num_instance_available_layers, nullptr));
        std::vector<VkLayerProperties> instance_supported_layers(num_instance_available_layers);
        VK_ASSERT(vkEnumerateInstanceLayerProperties(&num_instance_available_layers, instance_supported_layers.data()));

        uint32_t num_instance_available_extensions;
        VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &num_instance_available_extensions, nullptr));
        std::vector<VkExtensionProperties> instance_supported_extensions(num_instance_available_extensions);
        VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &num_instance_available_extensions, instance_supported_extensions.data()));

        std::vector<const char*> instance_required_layers;
        std::vector<const char*> instance_required_extensions;
        std::vector<const char*> instance_layers;
        std::vector<const char*> instance_extensions;

#if VULKAN_DEBUG
        if (IsLayerSupported("VK_LAYER_KHRONOS_validation", instance_supported_layers)) {
            instance_required_layers.push_back("VK_LAYER_KHRONOS_validation");
        } else {
            instance_required_layers.push_back("VK_LAYER_LUNARG_standard_validation");
        }
        bool debug_utils = false;
        if (IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, instance_supported_extensions)) {
            debug_utils = true;
            instance_required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        } else {
            instance_required_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
#endif
        instance_required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        instance_required_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

        for (auto it = instance_required_layers.begin(); it != instance_required_layers.end(); ++it) {
            if (IsLayerSupported(*it, instance_supported_layers)) {
                instance_layers.push_back(*it);
            }
        }

        for (auto it = instance_required_extensions.begin(); it != instance_required_extensions.end(); ++it) {
            if (IsExtensionSupported(*it, instance_supported_extensions)) {
                instance_extensions.push_back(*it);
            }
        }

        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pEngineName = "BlastEngine";
        app_info.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo ici;
        ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        ici.pNext = nullptr;
        ici.flags = 0;
        ici.pApplicationInfo = &app_info;
        ici.enabledLayerCount = instance_layers.size();
        ici.ppEnabledLayerNames = instance_layers.data();
        ici.enabledExtensionCount = instance_extensions.size();
        ici.ppEnabledExtensionNames = instance_extensions.data();

        VK_ASSERT(vkCreateInstance(&ici, nullptr, &instance));

        volkLoadInstance(instance);

#if VULKAN_DEBUG
        if (debug_utils) {
            VkDebugUtilsMessengerCreateInfoEXT dumci{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
            dumci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            dumci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            dumci.pfnUserCallback = DebugUtilsMessengerCallback;

            VK_ASSERT(vkCreateDebugUtilsMessengerEXT(instance, &dumci, nullptr, &debug_messenger));

        } else {
            VkDebugReportCallbackCreateInfoEXT drcci{VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT};
            drcci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            drcci.pfnCallback = DebugReportCallback;

            VK_ASSERT(vkCreateDebugReportCallbackEXT(instance, &drcci, nullptr, &debug_report_callback));
        }
#endif
        uint32_t num_gpus = 0;
        VK_ASSERT(vkEnumeratePhysicalDevices(instance, &num_gpus, nullptr));

        std::vector<VkPhysicalDevice> gpus(num_gpus);
        VK_ASSERT(vkEnumeratePhysicalDevices(instance, &num_gpus, gpus.data()));

        for (auto &g : gpus) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(g, &props);
            BLAST_LOGI("Found Vulkan GPU: %s\n", props.deviceName);
            BLAST_LOGI("API: %u.%u.%u\n",
                   VK_VERSION_MAJOR(props.apiVersion),
                   VK_VERSION_MINOR(props.apiVersion),
                   VK_VERSION_PATCH(props.apiVersion));
            BLAST_LOGI("Driver: %u.%u.%u\n",
                   VK_VERSION_MAJOR(props.driverVersion),
                   VK_VERSION_MINOR(props.driverVersion),
                   VK_VERSION_PATCH(props.driverVersion));
        }

        // todo: 支持多个GPU选择
        phy_device = gpus.front();

        vkGetPhysicalDeviceProperties(phy_device, &phy_device_properties);
        vkGetPhysicalDeviceFeatures(phy_device, &phy_device_features);
        vkGetPhysicalDeviceMemoryProperties(phy_device, &phy_device_memory_properties);

        uint32_t num_queue_families;
        vkGetPhysicalDeviceQueueFamilyProperties(phy_device, &num_queue_families, nullptr);

        queue_family_properties.resize(num_queue_families);
        vkGetPhysicalDeviceQueueFamilyProperties(phy_device, &num_queue_families, queue_family_properties.data());

        graphics_family = -1;
        compute_family = -1;
        copy_family = -1;
        for (uint32_t i = 0; i < (uint32_t)queue_family_properties.size(); i++) {
            if ((queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
                compute_family = i;
                break;
            }
        }

        for (uint32_t i = 0; i < (uint32_t)queue_family_properties.size(); i++) {
            if ((queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
                ((queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) {
                copy_family = i;
                break;
            }
        }

        for (uint32_t i = 0; i < (uint32_t)queue_family_properties.size(); i++) {
            if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphics_family = i;
                break;
            }
        }

        const float graphics_queue_prio = 0.0f;
        const float compute_queue_prio = 0.1f;
        const float transfer_queue_prio = 0.2f;

        // note:目前只使用三个queue
        std::vector<VkDeviceQueueCreateInfo> qci{};
        qci.resize(3);

        qci[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qci[0].queueFamilyIndex = graphics_family;
        qci[0].queueCount = 1;//queue_Family_properties[mGraphicsFamily].queueCount;
        qci[0].pQueuePriorities = &graphics_queue_prio;

        qci[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qci[1].queueFamilyIndex = compute_family;
        qci[1].queueCount = 1;//queue_Family_properties[mComputeFamily].queueCount;
        qci[1].pQueuePriorities = &compute_queue_prio;

        qci[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qci[2].queueFamilyIndex = copy_family;
        qci[2].queueCount = 1;//queue_Family_properties[mTransferFamily].queueCount;
        qci[2].pQueuePriorities = &transfer_queue_prio;

        uint32_t num_device_available_extensions = 0;
        VK_ASSERT(vkEnumerateDeviceExtensionProperties(phy_device, nullptr, &num_device_available_extensions, nullptr));

        std::vector<VkExtensionProperties> device_available_extensions(num_device_available_extensions);
        VK_ASSERT(vkEnumerateDeviceExtensionProperties(phy_device, nullptr, &num_device_available_extensions, device_available_extensions.data()));

        std::vector<const char*> device_required_extensions;
        std::vector<const char*> device_extensions;
        device_required_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);

        for (auto it = device_required_extensions.begin(); it != device_required_extensions.end(); ++it) {
            if (IsExtensionSupported(*it, device_available_extensions)) {
                device_extensions.push_back(*it);
            }
        }

        VkDeviceCreateInfo dci;
        dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        dci.pNext = nullptr;
        dci.flags = 0;
        dci.queueCreateInfoCount = qci.size();
        dci.pQueueCreateInfos = qci.data();
        dci.pEnabledFeatures = &phy_device_features;
        dci.enabledExtensionCount = device_extensions.size();
        dci.ppEnabledExtensionNames = device_extensions.data();
        dci.enabledLayerCount = 0;
        dci.ppEnabledLayerNames = nullptr;

        VK_ASSERT(vkCreateDevice(phy_device, &dci, nullptr, &device));

        vkGetDeviceQueue(device, graphics_family, 0, &graphics_queue);
        vkGetDeviceQueue(device, compute_family, 0, &compute_queue);
        vkGetDeviceQueue(device, copy_family, 0, &copy_queue);

        // resource manager
        resource_manager.device = device;
        resource_manager.instance = instance;

        // queues
        {
            queues[QUEUE_GRAPHICS].queue = graphics_queue;
            queues[QUEUE_COMPUTE].queue = compute_queue;

            VkSemaphoreTypeCreateInfo stci = {};
            stci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
            stci.pNext = nullptr;
            stci.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
            stci.initialValue = 0;

            VkSemaphoreCreateInfo sci = {};
            sci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            sci.pNext = &stci;
            sci.flags = 0;

            VK_ASSERT(vkCreateSemaphore(device, &sci, nullptr, &queues[QUEUE_GRAPHICS].semaphore));
            VK_ASSERT(vkCreateSemaphore(device, &sci, nullptr, &queues[QUEUE_COMPUTE].semaphore));
        }

        // frames
        for (uint32_t i = 0; i < BLAST_BUFFER_COUNT; ++i) {
            for (uint32_t j = 0; j < BLAST_CMD_COUNT; ++j) {
                frames[i].stage_buffers[j] = nullptr;
            }

            for (uint32_t j = 0; j < BLAST_QUEUE_COUNT; ++j) {
                VkFenceCreateInfo fci = {};
                fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                VK_ASSERT(vkCreateFence(device, &fci, nullptr, &frames[i].fence[j]));
            }
            VkCommandPoolCreateInfo cpci = {};
            cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cpci.queueFamilyIndex = graphics_family;
            cpci.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            VK_ASSERT(vkCreateCommandPool(device, &cpci, nullptr, &frames[i].init_command_pool));

            VkCommandBufferAllocateInfo cbai = {};
            cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cbai.commandBufferCount = 1;
            cbai.commandPool = frames[i].init_command_pool;
            cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            VK_ASSERT(vkAllocateCommandBuffers(device, &cbai, &frames[i].init_command_buffer));

            VkCommandBufferBeginInfo cbi = {};
            cbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            cbi.pInheritanceInfo = nullptr;

            VK_ASSERT(vkBeginCommandBuffer(frames[i].init_command_buffer, &cbi));
        }

        // CopyPool
        copy_pool.Init(this);
    }

    VulkanDevice::~VulkanDevice() {
        vkDeviceWaitIdle(device);

        // 清理
        for (auto& queue : queues) {
            vkDestroySemaphore(device, queue.semaphore, nullptr);
        }

        for (auto& frame : frames) {
            for (int queue = 0; queue < BLAST_QUEUE_COUNT; ++queue) {
                vkDestroyFence(device, frame.fence[queue], nullptr);
                for (int cmd = 0; cmd < BLAST_CMD_COUNT; ++cmd) {
                    vkDestroyCommandPool(device, frame.command_pools[cmd][queue], nullptr);

                    if (frame.stage_buffers[cmd]) {
                        frame.stage_buffers[cmd]->Destroy();
                        BLAST_SAFE_DELETE(frame.stage_buffers[cmd]);
                    }
                }
            }
            vkDestroyCommandPool(device, frame.init_command_pool, nullptr);

            for (auto& descriptormanager : frame.descriptor_pools) {
                descriptormanager.Destroy();
            }
        }

        copy_pool.Destroy();
        resource_manager.Clear();

        vkDestroyDevice(device, nullptr);

#if VULKAN_DEBUG
        if (debug_messenger != VK_NULL_HANDLE) {
            vkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
            debug_messenger = VK_NULL_HANDLE;
        }
        if (debug_report_callback != VK_NULL_HANDLE) {
            vkDestroyDebugReportCallbackEXT(instance, debug_report_callback, nullptr);
            debug_report_callback = VK_NULL_HANDLE;
        }
#endif
        vkDestroyInstance(instance, nullptr);
    }

    uint32_t VulkanDevice::FindMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties) {
        for (uint32_t i = 0; i < phy_device_memory_properties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (phy_device_memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        return -1;
    }

    GfxBuffer* VulkanDevice::CreateBuffer(const GfxBufferDesc& desc) {
        VulkanBuffer* internal_buffer = new VulkanBuffer();
        internal_buffer->desc = desc;

        VkBufferCreateInfo buffer_info;
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.pNext = nullptr;
        buffer_info.flags = 0;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffer_info.queueFamilyIndexCount = 0;
        buffer_info.pQueueFamilyIndices = nullptr;
        buffer_info.size = desc.size;
        buffer_info.usage = 0;

        if (desc.res_usage & RESOURCE_USAGE_RW_BUFFER) {
            buffer_info.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        if (desc.res_usage & RESOURCE_USAGE_UNIFORM_BUFFER) {
            buffer_info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        if (desc.res_usage & RESOURCE_USAGE_VERTEX_BUFFER) {
            buffer_info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }

        if (desc.res_usage & RESOURCE_USAGE_INDEX_BUFFER) {
            buffer_info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }

        if (desc.res_usage & RESOURCE_USAGE_INDIRECT_BUFFER) {
            buffer_info.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        VK_ASSERT(vkCreateBuffer(device, &buffer_info, nullptr, &internal_buffer->resource));

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(device, internal_buffer->resource, &memory_requirements);

        VkMemoryPropertyFlags memory_propertys;
        if (desc.mem_usage == MEMORY_USAGE_GPU_ONLY) {
            memory_propertys = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }

        if (desc.mem_usage == MEMORY_USAGE_CPU_TO_GPU) {
            memory_propertys = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }

        if (desc.mem_usage == MEMORY_USAGE_GPU_TO_CPU) {
            memory_propertys = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }

        VkMemoryAllocateInfo memory_allocate_info = {};
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.allocationSize = memory_requirements.size;
        memory_allocate_info.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, memory_propertys);

        VK_ASSERT(vkAllocateMemory(device, &memory_allocate_info, nullptr, &internal_buffer->memory));
        VK_ASSERT(vkBindBufferMemory(device, internal_buffer->resource, internal_buffer->memory, 0));

        return internal_buffer;
    }

    void VulkanDevice::DestroyBuffer(GfxBuffer* buffer) {
        resource_manager.destroy_locker.lock();
        VulkanBuffer* internal_buffer = (VulkanBuffer*)buffer;
        uint64_t frame_count = resource_manager.frame_count;
        resource_manager.destroyer_buffers.push_back(std::make_pair(std::make_pair(internal_buffer->resource, internal_buffer->memory), frame_count));
        BLAST_SAFE_DELETE(buffer);
        resource_manager.destroy_locker.unlock();
    }

    GfxTexture* VulkanDevice::CreateTexture(const GfxTextureDesc& desc) {
        VulkanTexture* internal_texture = new VulkanTexture();
        internal_texture->desc = desc;

        VkImageType image_type = VK_IMAGE_TYPE_MAX_ENUM;
        if (desc.depth > 1)
            image_type = VK_IMAGE_TYPE_3D;
        else if (desc.height > 1)
            image_type = VK_IMAGE_TYPE_2D;
        else
            image_type = VK_IMAGE_TYPE_1D;

        VkImageCreateInfo image_info = {};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.pNext = nullptr;
        image_info.imageType = image_type;
        image_info.format = ToVulkanFormat(desc.format);
        image_info.extent.width = desc.width;
        image_info.extent.height = desc.height;
        image_info.extent.depth = desc.depth;
        image_info.mipLevels = desc.num_levels;
        image_info.arrayLayers = desc.num_layers;
        image_info.samples = ToVulkanSampleCount(desc.sample_count);
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.queueFamilyIndexCount = 0;
        image_info.pQueueFamilyIndices = nullptr;
        image_info.flags = 0;
        if (RESOURCE_USAGE_TEXTURE_CUBE == (desc.res_usage & RESOURCE_USAGE_TEXTURE_CUBE)) {
            image_info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }
        if (image_type == VK_IMAGE_TYPE_3D) {
            image_info.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR;
        }

        image_info.usage = 0;
        if (desc.res_usage & RESOURCE_USAGE_SHADER_RESOURCE) {
            image_info.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (desc.res_usage & RESOURCE_USAGE_UNORDERED_ACCESS) {
            image_info.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
        }
        if (desc.res_usage & RESOURCE_USAGE_RENDER_TARGET) {
            image_info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (desc.res_usage & RESOURCE_USAGE_DEPTH_STENCIL) {
            image_info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        image_info.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        image_info.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        VK_ASSERT(vkCreateImage(device, &image_info, nullptr, &internal_texture->resource));

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements(device, internal_texture->resource, &memory_requirements);

        VkMemoryPropertyFlags memory_propertys;
        if (desc.mem_usage == MEMORY_USAGE_GPU_ONLY) {
            memory_propertys = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }
        if (desc.mem_usage == MEMORY_USAGE_CPU_TO_GPU) {
            memory_propertys = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }

        if (desc.mem_usage == MEMORY_USAGE_GPU_TO_CPU) {
            memory_propertys = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }

        VkMemoryAllocateInfo mai = {};
        mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mai.allocationSize = memory_requirements.size;
        mai.memoryTypeIndex = FindMemoryType(memory_requirements.memoryTypeBits, memory_propertys);

        VK_ASSERT(vkAllocateMemory(device, &mai, nullptr, &internal_texture->memory));
        VK_ASSERT(vkBindImageMemory(device, internal_texture->resource, internal_texture->memory, 0));

        if (desc.res_usage & RESOURCE_USAGE_SHADER_RESOURCE) {
            CreateSubresource(internal_texture, SRV, 0, -1, 0, -1);
        }
        if (desc.res_usage & RESOURCE_USAGE_UNORDERED_ACCESS) {
            CreateSubresource(internal_texture, UAV, 0, -1, 0, -1);
        }
        if (desc.res_usage & RESOURCE_USAGE_RENDER_TARGET) {
            CreateSubresource(internal_texture, RTV, 0, -1, 0, -1);
        }
        if (desc.res_usage & RESOURCE_USAGE_DEPTH_STENCIL) {
            CreateSubresource(internal_texture, DSV, 0, -1, 0, -1);
        }

        return internal_texture;
    }

    void VulkanDevice::DestroyTexture(GfxTexture* texture) {
        resource_manager.destroy_locker.lock();
        VulkanTexture* internal_texture = (VulkanTexture*)texture;
        uint64_t frame_count = resource_manager.frame_count;
        resource_manager.destroyer_images.push_back(std::make_pair(std::make_pair(internal_texture->resource, internal_texture->memory), frame_count));

        if (internal_texture->srv != VK_NULL_HANDLE) {
            resource_manager.destroyer_imageviews.push_back(std::make_pair(internal_texture->srv, frame_count));
        }
        if (internal_texture->uav != VK_NULL_HANDLE) {
            resource_manager.destroyer_imageviews.push_back(std::make_pair(internal_texture->uav, frame_count));
        }
        if (internal_texture->rtv != VK_NULL_HANDLE) {
            resource_manager.destroyer_imageviews.push_back(std::make_pair(internal_texture->rtv, frame_count));
        }
        if (internal_texture->dsv != VK_NULL_HANDLE) {
            resource_manager.destroyer_imageviews.push_back(std::make_pair(internal_texture->dsv, frame_count));
        }

        for (auto x : internal_texture->subresources_srv) {
            resource_manager.destroyer_imageviews.push_back(std::make_pair(x, frame_count));
        }

        for (auto x : internal_texture->subresources_uav) {
            resource_manager.destroyer_imageviews.push_back(std::make_pair(x, frame_count));
        }

        for (auto x : internal_texture->subresources_rtv) {
            resource_manager.destroyer_imageviews.push_back(std::make_pair(x, frame_count));
        }

        for (auto x : internal_texture->subresources_dsv) {
            resource_manager.destroyer_imageviews.push_back(std::make_pair(x, frame_count));
        }

        BLAST_SAFE_DELETE(texture);
        resource_manager.destroy_locker.unlock();
    }

    int32_t VulkanDevice::CreateSubresource(GfxBuffer* buffer, SubResourceType type, uint32_t size, uint32_t offset) {
        return -1;
    }

    int32_t VulkanDevice::CreateSubresource(GfxTexture* texture, SubResourceType type, uint32_t first_slice, uint32_t slice_count, uint32_t first_mip, uint32_t mip_count) {
        VulkanTexture* internal_texture = (VulkanTexture*) texture;

        VkImageViewCreateInfo ivci = {};
        ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivci.flags = 0;
        ivci.image = internal_texture->resource;
        ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ivci.subresourceRange.baseArrayLayer = first_slice;
        ivci.subresourceRange.layerCount = slice_count;
        ivci.subresourceRange.baseMipLevel = first_mip;
        ivci.subresourceRange.levelCount = mip_count;
        ivci.format = ToVulkanFormat(texture->desc.format);

        VkImageType image_type = VK_IMAGE_TYPE_MAX_ENUM;
        if (texture->desc.depth > 1)
            image_type = VK_IMAGE_TYPE_3D;
        else if (texture->desc.height > 1)
            image_type = VK_IMAGE_TYPE_2D;
        else
            image_type = VK_IMAGE_TYPE_1D;

        VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
        switch (image_type) {
            case VK_IMAGE_TYPE_1D:
                view_type = texture->desc.num_layers > 1 ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
                break;
            case VK_IMAGE_TYPE_2D:
                if (RESOURCE_USAGE_TEXTURE_CUBE == (texture->desc.res_usage & RESOURCE_USAGE_TEXTURE_CUBE))
                    view_type = (texture->desc.num_layers > 6) ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
                else
                    view_type = texture->desc.num_layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
                break;
            case VK_IMAGE_TYPE_3D:
                if (texture->desc.num_layers > 1)
                    assert(false);

                view_type = VK_IMAGE_VIEW_TYPE_3D;
                break;
            default:
                assert(false && "image format not supported!");
                break;
        }
        ivci.viewType = view_type;

        switch (type) {
            case SRV: {
                switch (ivci.format) {
                    case VK_FORMAT_D16_UNORM:
                        ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                        break;
                    case VK_FORMAT_D32_SFLOAT:
                        ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                        break;
                    case VK_FORMAT_D24_UNORM_S8_UINT:
                        ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                        break;
                    case VK_FORMAT_D32_SFLOAT_S8_UINT:
                        ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                        break;
                }

                VkImageView srv;
                VK_ASSERT(vkCreateImageView(device, &ivci, nullptr, &srv));

                if (internal_texture->srv == VK_NULL_HANDLE) {
                    internal_texture->srv = srv;
                    return -1;
                }
                internal_texture->subresources_srv.push_back(srv);
                return int32_t(internal_texture->subresources_srv.size() - 1);
            }
                break;
            case UAV: {
                if (ivci.viewType == VK_IMAGE_VIEW_TYPE_CUBE || ivci.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY) {
                    ivci.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                }

                VkImageView uav;
                VK_ASSERT(vkCreateImageView(device, &ivci, nullptr, &uav));

                if (internal_texture->uav == VK_NULL_HANDLE) {
                    internal_texture->uav = uav;
                    return -1;
                }
                internal_texture->subresources_uav.push_back(uav);
                return int32_t(internal_texture->subresources_uav.size() - 1);
            }
                break;
            case RTV: {
                VkImageView rtv;
                ivci.subresourceRange.levelCount = 1;
                VK_ASSERT(vkCreateImageView(device, &ivci, nullptr, &rtv));

                if (internal_texture->rtv == VK_NULL_HANDLE) {
                    internal_texture->rtv = rtv;
                    return -1;
                }
                internal_texture->subresources_rtv.push_back(rtv);
                return int32_t(internal_texture->subresources_rtv.size() - 1);
            }
                break;
            case DSV: {
                ivci.subresourceRange.levelCount = 1;
                ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

                switch (texture->desc.format) {
                    case FORMAT_D24_UNORM_S8_UINT:
                        ivci.format = VK_FORMAT_D24_UNORM_S8_UINT;
                        ivci.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                        break;
                }

                VkImageView dsv;
                VK_ASSERT(vkCreateImageView(device, &ivci, nullptr, &dsv));

                if (internal_texture->dsv == VK_NULL_HANDLE) {
                    internal_texture->dsv = dsv;
                    return -1;
                }
                internal_texture->subresources_dsv.push_back(dsv);
                return int32_t(internal_texture->subresources_dsv.size() - 1);
            }
                break;
            default:
                break;
        }
        return -1;
    }

    GfxSampler* VulkanDevice::CreateSampler(const GfxSamplerDesc& desc) {
        VulkanSampler* internal_sampler = new VulkanSampler();
        internal_sampler->desc = desc;

        VkSamplerCreateInfo sci = {};
        sci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sci.magFilter = ToVulkanFilter(desc.mag_filter);
        sci.minFilter = ToVulkanFilter(desc.min_filter);
        sci.mipmapMode = ToVulkanMipmapMode(desc.mipmap_mode);
        sci.addressModeU = ToVulkanAddressMode(desc.address_u);
        sci.addressModeV = ToVulkanAddressMode(desc.address_v);
        sci.addressModeW = ToVulkanAddressMode(desc.address_w);
        sci.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        sci.anisotropyEnable = VK_FALSE;
        sci.maxAnisotropy = 0.0f;
        sci.unnormalizedCoordinates = VK_FALSE;
        sci.compareEnable = VK_FALSE;
        sci.compareOp = VK_COMPARE_OP_ALWAYS;
        VK_ASSERT(vkCreateSampler(device, &sci, nullptr, &internal_sampler->sampler));

        return internal_sampler;
    }

    void VulkanDevice::DestroySampler(GfxSampler* sampler) {
        resource_manager.destroy_locker.lock();
        VulkanSampler* internal_sampler = (VulkanSampler*)sampler;
        uint64_t frame_count = resource_manager.frame_count;
        resource_manager.destroyer_samplers.push_back(std::make_pair(internal_sampler->sampler, frame_count));
        BLAST_SAFE_DELETE(sampler);
        resource_manager.destroy_locker.unlock();
    }

    GfxSwapChain* VulkanDevice::CreateSwapChain(const GfxSwapChainDesc& desc, GfxSwapChain* old_swapchain) {
        VulkanSwapChain* internal_swapchain = nullptr;
        if (old_swapchain) {
            internal_swapchain = (VulkanSwapChain*)old_swapchain;
        } else {
            internal_swapchain = new VulkanSwapChain();
        }
        internal_swapchain->desc = desc;

        if(internal_swapchain->surface == VK_NULL_HANDLE) {
#ifdef WIN32
            VkWin32SurfaceCreateInfoKHR sci = {};
            sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            sci.pNext = nullptr;
            sci.flags = 0;
            sci.hinstance = ::GetModuleHandle(nullptr);
            sci.hwnd = (HWND)desc.window;
            VK_ASSERT(vkCreateWin32SurfaceKHR(instance, &sci, nullptr, &internal_swapchain->surface));
#endif
        }

        // get present family
        uint32_t present_family = VK_QUEUE_FAMILY_IGNORED;
        uint32_t family_index = 0;
        for (const auto& queue_family : queue_family_properties) {
            VkBool32 present_support = false;
            VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(phy_device, (uint32_t)family_index, internal_swapchain->surface, &present_support));

            if (present_family == VK_QUEUE_FAMILY_IGNORED && queue_family.queueCount > 0 && present_support) {
                present_family = family_index;
                break;
            }

            family_index++;
        }

        if (present_family == VK_QUEUE_FAMILY_IGNORED) {
            return nullptr;
        }

        VkSurfaceCapabilitiesKHR swapchain_capabilities;
        VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phy_device, internal_swapchain->surface, &swapchain_capabilities));

        uint32_t format_count;
        VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(phy_device, internal_swapchain->surface, &format_count, nullptr));

        std::vector<VkSurfaceFormatKHR> swapchain_formats(format_count);
        VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(phy_device, internal_swapchain->surface, &format_count, swapchain_formats.data()));

        uint32_t present_mode_count;
        VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(phy_device, internal_swapchain->surface, &present_mode_count, nullptr));

        std::vector<VkPresentModeKHR> swapchain_present_modes(present_mode_count);
        VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(phy_device, internal_swapchain->surface, &present_mode_count, swapchain_present_modes.data()));

        // todo: using outside format
        VkSurfaceFormatKHR surface_format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;

        internal_swapchain->swapchain_extent = {};
        internal_swapchain->swapchain_extent.width = std::max(swapchain_capabilities.minImageExtent.width, swapchain_capabilities.maxImageExtent.width);
        internal_swapchain->swapchain_extent.height = std::max(swapchain_capabilities.minImageExtent.height, swapchain_capabilities.maxImageExtent.height);

        uint32_t image_count = desc.buffer_count;
        if ((swapchain_capabilities.maxImageCount > 0) && (image_count > swapchain_capabilities.maxImageCount)) {
            image_count = swapchain_capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR scci = {};
        scci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        scci.surface = internal_swapchain->surface;
        scci.minImageCount = image_count;
        scci.imageFormat = surface_format.format;
        scci.imageColorSpace = surface_format.colorSpace;
        scci.imageExtent = internal_swapchain->swapchain_extent;
        scci.imageArrayLayers = 1;
        scci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        scci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // transform
        if(swapchain_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
            scci.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        else
            scci.preTransform = swapchain_capabilities.currentTransform;

        // composite alpha
        std::vector<VkCompositeAlphaFlagBitsKHR> composite_alpha_flags = {
                VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
                VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
                VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        };
        for (auto& composite_alpha_flag : composite_alpha_flags) {
            if (swapchain_capabilities.supportedCompositeAlpha & composite_alpha_flag) {
                scci.compositeAlpha = composite_alpha_flag;
                break;
            };
        }

        scci.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        if (!desc.vsync) {
            for (auto& presentMode : swapchain_present_modes) {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    scci.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                    break;
                }
                if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                    scci.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                }
            }
        }
        scci.clipped = VK_TRUE;
        scci.oldSwapchain = internal_swapchain->swapchain;
        VK_ASSERT(vkCreateSwapchainKHR(device, &scci, nullptr, &internal_swapchain->swapchain));

        if (scci.oldSwapchain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(device, scci.oldSwapchain, nullptr);
        }

        // images
        VK_ASSERT(vkGetSwapchainImagesKHR(device, internal_swapchain->swapchain, &image_count, nullptr));
        internal_swapchain->swapchain_images.resize(image_count);
        VK_ASSERT(vkGetSwapchainImagesKHR(device, internal_swapchain->swapchain, &image_count, internal_swapchain->swapchain_images.data()));
        internal_swapchain->swapchain_image_format = surface_format.format;
        
        // Create default render pass:
        {
            VkAttachmentDescription color_attachment = {};
            color_attachment.format = surface_format.format;
            color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference color_attachment_ref = {};
            color_attachment_ref.attachment = 0;
            color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass = {};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &color_attachment_ref;

            VkRenderPassCreateInfo rpci = {};
            rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            rpci.attachmentCount = 1;
            rpci.pAttachments = &color_attachment;
            rpci.subpassCount = 1;
            rpci.pSubpasses = &subpass;

            VkSubpassDependency dependency = {};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            rpci.dependencyCount = 1;
            rpci.pDependencies = &dependency;
            if (internal_swapchain->renderpass != VK_NULL_HANDLE) {
                resource_manager.destroyer_renderpasses.push_back(std::make_pair(internal_swapchain->renderpass, resource_manager.frame_count));
            }

            VK_ASSERT(vkCreateRenderPass(device, &rpci, nullptr, &internal_swapchain->renderpass));

            // 计算renderpass的hash
            internal_swapchain->renderpass_hash = 0;
            hash_combine(internal_swapchain->renderpass_hash, internal_swapchain->swapchain_image_format);
        }

        // create swap chain render targets
        internal_swapchain->swapchain_image_views.resize(internal_swapchain->swapchain_images.size());
        internal_swapchain->swapchain_framebuffers.resize(internal_swapchain->swapchain_images.size());
        for (uint32_t i = 0; i < internal_swapchain->swapchain_images.size(); ++i) {
            VkImageViewCreateInfo ivci = {};
            ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            ivci.image = internal_swapchain->swapchain_images[i];
            ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ivci.format = internal_swapchain->swapchain_image_format;
            ivci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ivci.subresourceRange.baseMipLevel = 0;
            ivci.subresourceRange.levelCount = 1;
            ivci.subresourceRange.baseArrayLayer = 0;
            ivci.subresourceRange.layerCount = 1;

            if (internal_swapchain->swapchain_image_views[i] != VK_NULL_HANDLE) {
                resource_manager.destroyer_imageviews.push_back(std::make_pair(internal_swapchain->swapchain_image_views[i], resource_manager.frame_count));
            }
            VK_ASSERT(vkCreateImageView(device, &ivci, nullptr, &internal_swapchain->swapchain_image_views[i]));

            VkImageView attachments[] = { internal_swapchain->swapchain_image_views[i] };

            VkFramebufferCreateInfo fci = {};
            fci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fci.renderPass = internal_swapchain->renderpass;
            fci.attachmentCount = 1;
            fci.pAttachments = attachments;
            fci.width = internal_swapchain->swapchain_extent.width;
            fci.height = internal_swapchain->swapchain_extent.height;
            fci.layers = 1;

            if (internal_swapchain->swapchain_framebuffers[i] != VK_NULL_HANDLE) {
                resource_manager.destroyer_framebuffers.push_back(std::make_pair(internal_swapchain->swapchain_framebuffers[i], resource_manager.frame_count));
            }
            VK_ASSERT(vkCreateFramebuffer(device, &fci, nullptr, &internal_swapchain->swapchain_framebuffers[i]));
        }


        VkSemaphoreCreateInfo sci = {};
        sci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (internal_swapchain->swapchain_acquire_semaphore == VK_NULL_HANDLE) {
            VK_ASSERT(vkCreateSemaphore(device, &sci, nullptr, &internal_swapchain->swapchain_acquire_semaphore));
        }

        if (internal_swapchain->swapchain_release_semaphore == VK_NULL_HANDLE) {
            VK_ASSERT(vkCreateSemaphore(device, &sci, nullptr, &internal_swapchain->swapchain_release_semaphore));
        }

        return internal_swapchain;
    }

    void VulkanDevice::DestroySwapChain(GfxSwapChain* swapchain) {
        vkDeviceWaitIdle(device);
        VulkanSwapChain* internal_swapchain = (VulkanSwapChain*)swapchain;
        for (size_t i = 0; i < internal_swapchain->swapchain_images.size(); ++i) {
            vkDestroyFramebuffer(device, internal_swapchain->swapchain_framebuffers[i], nullptr);
            vkDestroyImageView(device, internal_swapchain->swapchain_image_views[i], nullptr);
        }
        vkDestroyRenderPass(device, internal_swapchain->renderpass, nullptr);

        vkDestroySwapchainKHR(device, internal_swapchain->swapchain, nullptr);
        vkDestroySurfaceKHR(instance, internal_swapchain->surface, nullptr);
        vkDestroySemaphore(device, internal_swapchain->swapchain_acquire_semaphore, nullptr);
        vkDestroySemaphore(device, internal_swapchain->swapchain_release_semaphore, nullptr);
        BLAST_SAFE_DELETE(swapchain);
    }

    GfxRenderPass* VulkanDevice::CreateRenderPass(const GfxRenderPassDesc& desc) {
        auto internal_renderpass = new VulkanRenderPass();
        internal_renderpass->desc = desc;

        internal_renderpass->hash = 0;
        hash_combine(internal_renderpass->hash, desc.attachments.size());
        for (auto& attachment : desc.attachments) {
            if (attachment.type == RenderPassAttachment::RENDERTARGET || attachment.type == RenderPassAttachment::DEPTH_STENCIL) {
                hash_combine(internal_renderpass->hash, attachment.texture->desc.format);
                hash_combine(internal_renderpass->hash, attachment.texture->desc.sample_count);
            }
        }

        VkImageView attachments[18] = {};
        VkAttachmentDescription2 attachment_descriptions[18] = {};
        VkAttachmentReference2 color_attachment_refs[8] = {};
        VkAttachmentReference2 resolve_attachment_refs[8] = {};
        VkAttachmentReference2 depth_attachment_ref = {};

        uint32_t resolve_count = 0;

        VkSubpassDescription2 subpass = {};
        subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        uint32_t valid_attachment_count = 0;
        for (auto& attachment : desc.attachments) {
            const GfxTexture* texture = attachment.texture;
            const GfxTextureDesc& texdesc = texture->desc;
            int subresource = attachment.subresource;
            VulkanTexture* internal_texture = (VulkanTexture*)texture;

            attachment_descriptions[valid_attachment_count].sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
            attachment_descriptions[valid_attachment_count].format = ToVulkanFormat(texdesc.format);
            attachment_descriptions[valid_attachment_count].samples = (VkSampleCountFlagBits)texdesc.sample_count;

            switch (attachment.loadop) {
                default:
                case LOAD_LOAD:
                    attachment_descriptions[valid_attachment_count].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                    break;
                case LOAD_CLEAR:
                    attachment_descriptions[valid_attachment_count].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    break;
                case LOAD_DONTCARE:
                    attachment_descriptions[valid_attachment_count].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                    break;
            }

            switch (attachment.storeop) {
                default:
                case STORE_STORE:
                    attachment_descriptions[valid_attachment_count].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                    break;
                case STORE_DONTCARE:
                    attachment_descriptions[valid_attachment_count].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                    break;
            }

            attachment_descriptions[valid_attachment_count].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment_descriptions[valid_attachment_count].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

            if (attachment.type == RenderPassAttachment::RENDERTARGET) {
                if (subresource < 0) {
                    attachments[valid_attachment_count] = internal_texture->rtv;
                } else {
                    attachments[valid_attachment_count] = internal_texture->subresources_rtv[subresource];
                }

                attachment_descriptions[valid_attachment_count].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachment_descriptions[valid_attachment_count].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                color_attachment_refs[subpass.colorAttachmentCount].sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                color_attachment_refs[subpass.colorAttachmentCount].attachment = valid_attachment_count;
                color_attachment_refs[subpass.colorAttachmentCount].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                color_attachment_refs[subpass.colorAttachmentCount].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                subpass.colorAttachmentCount++;
                subpass.pColorAttachments = color_attachment_refs;
            }
            else if (attachment.type == RenderPassAttachment::DEPTH_STENCIL) {
                if (subresource < 0) {
                    attachments[valid_attachment_count] = internal_texture->dsv;
                } else {
                    attachments[valid_attachment_count] = internal_texture->subresources_dsv[subresource];
                }

                attachment_descriptions[valid_attachment_count].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachment_descriptions[valid_attachment_count].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                depth_attachment_ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
                depth_attachment_ref.attachment = valid_attachment_count;
                depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                depth_attachment_ref.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                subpass.pDepthStencilAttachment = &depth_attachment_ref;

                if (IsFormatStencilSupport(texdesc.format)) {
                    depth_attachment_ref.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                    switch (attachment.loadop) {
                        default:
                        case LOAD_LOAD:
                            attachment_descriptions[valid_attachment_count].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                            break;
                        case LOAD_CLEAR:
                            attachment_descriptions[valid_attachment_count].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                            break;
                        case LOAD_DONTCARE:
                            attachment_descriptions[valid_attachment_count].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                            break;
                    }

                    switch (attachment.storeop) {
                        default:
                        case STORE_STORE:
                            attachment_descriptions[valid_attachment_count].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
                            break;
                        case STORE_DONTCARE:
                            attachment_descriptions[valid_attachment_count].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                            break;
                    }
                }
            }
            else if (attachment.type == RenderPassAttachment::RESOLVE) {
                attachment_descriptions[valid_attachment_count].initialLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                attachment_descriptions[valid_attachment_count].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

                resolve_attachment_refs[resolve_count].sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;

                if (attachment.texture == nullptr) {
                    resolve_attachment_refs[resolve_count].attachment = VK_ATTACHMENT_UNUSED;
                }
                else {
                    if (subresource < 0) {
                        attachments[valid_attachment_count] = internal_texture->srv;
                    } else {
                        attachments[valid_attachment_count] = internal_texture->subresources_srv[subresource];
                    }

                    resolve_attachment_refs[resolve_count].attachment = valid_attachment_count;
                    resolve_attachment_refs[resolve_count].layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    resolve_attachment_refs[resolve_count].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                }

                resolve_count++;
                subpass.pResolveAttachments = resolve_attachment_refs;
            }

            valid_attachment_count++;
        }
        assert(desc.attachments.size() == valid_attachment_count);

        VkRenderPassCreateInfo2 rpci = {};
        rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
        rpci.attachmentCount = valid_attachment_count;
        rpci.pAttachments = attachment_descriptions;
        rpci.subpassCount = 1;
        rpci.pSubpasses = &subpass;
        VK_ASSERT(vkCreateRenderPass2(device, &rpci, nullptr, &internal_renderpass->renderpass));

        const GfxTextureDesc& texdesc = desc.attachments[0].texture->desc;
        auto internal_texture = (VulkanTexture*)desc.attachments[0].texture;
        VkFramebufferCreateInfo fci = {};
        fci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fci.renderPass = internal_renderpass->renderpass;
        fci.attachmentCount = valid_attachment_count;
        fci.pAttachments = attachments;
        fci.width = texdesc.width;
        fci.height = texdesc.height;
        fci.layers = 1;
        VK_ASSERT(vkCreateFramebuffer(device, &fci, nullptr, &internal_renderpass->framebuffer));

        internal_renderpass->begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        internal_renderpass->begin_info.renderPass = internal_renderpass->renderpass;
        internal_renderpass->begin_info.framebuffer = internal_renderpass->framebuffer;
        internal_renderpass->begin_info.renderArea.offset = { 0, 0 };
        internal_renderpass->begin_info.renderArea.extent.width = fci.width;
        internal_renderpass->begin_info.renderArea.extent.height = fci.height;
        internal_renderpass->begin_info.clearValueCount = valid_attachment_count;
        internal_renderpass->begin_info.pClearValues = internal_renderpass->clear_colors;

        uint32_t i = 0;
        for (auto& attachment : desc.attachments) {
            if (desc.attachments[i].type == RenderPassAttachment::RESOLVE || attachment.texture == nullptr)
                continue;

            const ClearValue& clear = desc.attachments[i].texture->desc.clear;
            if (desc.attachments[i].type == RenderPassAttachment::RENDERTARGET) {
                internal_renderpass->clear_colors[i].color.float32[0] = clear.color[0];
                internal_renderpass->clear_colors[i].color.float32[1] = clear.color[1];
                internal_renderpass->clear_colors[i].color.float32[2] = clear.color[2];
                internal_renderpass->clear_colors[i].color.float32[3] = clear.color[3];
            }
            else if (desc.attachments[i].type == RenderPassAttachment::DEPTH_STENCIL) {
                internal_renderpass->clear_colors[i].depthStencil.depth = clear.depthstencil.depth;
                internal_renderpass->clear_colors[i].depthStencil.stencil = clear.depthstencil.stencil;
            }
            else {
                assert(0);
            }
            i++;
        }
        return internal_renderpass;
    }

    void VulkanDevice::DestroyRenderPass(GfxRenderPass* renderpass) {
        resource_manager.destroy_locker.lock();
        VulkanRenderPass* internal_renderpass = (VulkanRenderPass*)renderpass;
        uint64_t frame_count = resource_manager.frame_count;
        resource_manager.destroyer_renderpasses.push_back(std::make_pair(internal_renderpass->renderpass, frame_count));
        resource_manager.destroyer_framebuffers.push_back(std::make_pair(internal_renderpass->framebuffer, frame_count));
        BLAST_SAFE_DELETE(renderpass);
        resource_manager.destroy_locker.unlock();
    }

    GfxShader* VulkanDevice::CreateShader(const GfxShaderDesc& desc) {
        VulkanShader* internal_shader = new VulkanShader();
        internal_shader->stage = desc.stage;

        VkShaderModuleCreateInfo smci = {};
        smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        smci.codeSize = desc.bytecode_length;
        smci.pCode = (const uint32_t*)desc.bytecode;
        VK_ASSERT(vkCreateShaderModule(device, &smci, nullptr, &internal_shader->shader_module));

        internal_shader->stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        internal_shader->stage_info.module = internal_shader->shader_module;
        internal_shader->stage_info.pName = "main";
        switch (desc.stage) {
            case SHADER_STAGE_VERT:
                internal_shader->stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
                break;
            case SHADER_STAGE_HULL:
                internal_shader->stage_info.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                break;
            case SHADER_STAGE_DOMN:
                internal_shader->stage_info.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                break;
            case SHADER_STAGE_GEOM:
                internal_shader->stage_info.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                break;
            case SHADER_STAGE_FRAG:
                internal_shader->stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            case SHADER_STAGE_COMP:
                internal_shader->stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
                break;
            default:
                internal_shader->stage_info.stage = VK_SHADER_STAGE_ALL;
                break;
        }

        {
            // 获取反射信息
            SpvReflectShaderModule module;
            SpvReflectResult result = spvReflectCreateShaderModule(smci.codeSize, smci.pCode, &module);
            assert(result == SPV_REFLECT_RESULT_SUCCESS);

            uint32_t binding_count = 0;
            result = spvReflectEnumerateDescriptorBindings(
                    &module, &binding_count, nullptr
            );
            assert(result == SPV_REFLECT_RESULT_SUCCESS);

            std::vector<SpvReflectDescriptorBinding*> bindings(binding_count);
            result = spvReflectEnumerateDescriptorBindings(
                    &module, &binding_count, bindings.data()
            );
            assert(result == SPV_REFLECT_RESULT_SUCCESS);

            uint32_t push_count = 0;
            result = spvReflectEnumeratePushConstantBlocks(&module, &push_count, nullptr);
            assert(result == SPV_REFLECT_RESULT_SUCCESS);

            std::vector<SpvReflectBlockVariable*> pushconstants(push_count);
            result = spvReflectEnumeratePushConstantBlocks(&module, &push_count, pushconstants.data());
            assert(result == SPV_REFLECT_RESULT_SUCCESS);

            std::vector<VkSampler> staticsamplers;

            for (auto& x : pushconstants) {
                auto& push = internal_shader->pushconstants;
                push.stageFlags = internal_shader->stage_info.stage;
                push.offset = x->offset;
                push.size = x->size;
            }

            for (auto& x : bindings) {
                VkDescriptorSetLayoutBinding descriptor = {};
                descriptor.stageFlags = internal_shader->stage_info.stage;
                descriptor.binding = x->binding;
                descriptor.descriptorCount = x->count;
                descriptor.descriptorType = (VkDescriptorType)x->descriptor_type;

                VkImageViewType image_view_type;
                image_view_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;

                switch (x->descriptor_type) {
                    default:
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                        switch (x->image.dim) {
                            default:
                            case SpvDim1D:
                                if (x->image.arrayed == 0)
                                {
                                    image_view_type = VK_IMAGE_VIEW_TYPE_1D;
                                }
                                else
                                {
                                    image_view_type = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                                }
                                break;
                            case SpvDim2D:
                                if (x->image.arrayed == 0)
                                {
                                    image_view_type = VK_IMAGE_VIEW_TYPE_2D;
                                }
                                else
                                {
                                    image_view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                                }
                                break;
                            case SpvDim3D:
                                image_view_type = VK_IMAGE_VIEW_TYPE_3D;
                                break;
                            case SpvDimCube:
                                if (x->image.arrayed == 0)
                                {
                                    image_view_type = VK_IMAGE_VIEW_TYPE_CUBE;
                                }
                                else
                                {
                                    image_view_type = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
                                }
                                break;
                        }
                        break;
                }

                internal_shader->layout_bindings.push_back(descriptor);
                internal_shader->image_view_types.push_back(image_view_type);
            }

            spvReflectDestroyShaderModule(&module);

            if (desc.stage == SHADER_STAGE_COMP || desc.stage == SHADER_STAGE_RAYTRACING) {
                std::vector<VkDescriptorSetLayout> layouts;
                {
                    VkDescriptorSetLayoutCreateInfo dslci = {};
                    dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    dslci.pBindings = internal_shader->layout_bindings.data();
                    dslci.bindingCount = uint32_t(internal_shader->layout_bindings.size());
                    VK_ASSERT(vkCreateDescriptorSetLayout(device, &dslci, nullptr, &internal_shader->descriptor_set_layout));
                    layouts.push_back(internal_shader->descriptor_set_layout);
                }

                VkPipelineLayoutCreateInfo plci = {};
                plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                plci.pSetLayouts = layouts.data();
                plci.setLayoutCount = (uint32_t)layouts.size();
                if (internal_shader->pushconstants.size > 0) {
                    plci.pushConstantRangeCount = 1;
                    plci.pPushConstantRanges = &internal_shader->pushconstants;
                }
                else {
                    plci.pushConstantRangeCount = 0;
                    plci.pPushConstantRanges = nullptr;
                }

                VK_ASSERT(vkCreatePipelineLayout(device, &plci, nullptr, &internal_shader->pipeline_layout_cs));
            }
        }

        if (desc.stage == SHADER_STAGE_COMP) {
            VkComputePipelineCreateInfo pipelineInfo = {};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            pipelineInfo.layout = internal_shader->pipeline_layout_cs;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo.stage = internal_shader->stage_info;
            VK_ASSERT(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &internal_shader->pipeline_cs));
        }

        return internal_shader;
    }

    void VulkanDevice::DestroyShader(GfxShader* shader) {
        resource_manager.destroy_locker.lock();
        VulkanShader* internal_shader = (VulkanShader*)shader;
        uint64_t frame_count = resource_manager.frame_count;
        resource_manager.destroyer_shadermodules.push_back(std::make_pair(internal_shader->shader_module, frame_count));
        if (internal_shader->pipeline_cs) {
            resource_manager.destroyer_pipelines.push_back(std::make_pair(internal_shader->pipeline_cs, frame_count));
            resource_manager.destroyer_pipeline_layouts.push_back(std::make_pair(internal_shader->pipeline_layout_cs, frame_count));
            resource_manager.destroyer_descriptor_set_layouts.push_back(std::make_pair(internal_shader->descriptor_set_layout, frame_count));
        }
        BLAST_SAFE_DELETE(shader);
        resource_manager.destroy_locker.unlock();
    }

    GfxPipeline* VulkanDevice::CreatePipeline(const GfxPipelineDesc& desc) {
        VulkanPipeline* internal_pipeline = new VulkanPipeline();
        internal_pipeline->desc = desc;

        internal_pipeline->hash = 0;
        hash_combine(internal_pipeline->hash, desc.sc);
        hash_combine(internal_pipeline->hash, desc.rp);
        hash_combine(internal_pipeline->hash, desc.vs);
        hash_combine(internal_pipeline->hash, desc.fs);
        hash_combine(internal_pipeline->hash, desc.hs);
        hash_combine(internal_pipeline->hash, desc.ds);
        hash_combine(internal_pipeline->hash, desc.gs);
        hash_combine(internal_pipeline->hash, desc.il);
        hash_combine(internal_pipeline->hash, desc.rs);
        hash_combine(internal_pipeline->hash, desc.bs);
        hash_combine(internal_pipeline->hash, desc.dss);
        hash_combine(internal_pipeline->hash, desc.primitive_topo);
        hash_combine(internal_pipeline->hash, desc.patch_control_points);

        {
            auto insert_shader = [&](const GfxShader* shader) {
                if (shader == nullptr)
                    return;

                VulkanShader* internal_shader = (VulkanShader*)shader;
                uint32_t i = 0;
                for (auto& x : internal_shader->layout_bindings) {
                    bool found = false;
                    for (auto& y : internal_pipeline->layout_bindings) {
                        if (x.binding == y.binding) {
                            assert(x.descriptorCount == y.descriptorCount);
                            assert(x.descriptorType == y.descriptorType);
                            found = true;
                            y.stageFlags |= x.stageFlags;
                            break;
                        }
                    }

                    if (!found) {
                        internal_pipeline->layout_bindings.push_back(x);
                        internal_pipeline->image_view_types.push_back(internal_shader->image_view_types[i]);
                    }
                    i++;
                }

                if (internal_shader->pushconstants.size > 0) {
                    internal_pipeline->pushconstants.offset = std::min(internal_pipeline->pushconstants.offset, internal_shader->pushconstants.offset);
                    internal_pipeline->pushconstants.size = std::max(internal_pipeline->pushconstants.size, internal_shader->pushconstants.size);
                    internal_pipeline->pushconstants.stageFlags |= internal_shader->pushconstants.stageFlags;
                }
            };

            insert_shader(desc.vs);
            insert_shader(desc.hs);
            insert_shader(desc.ds);
            insert_shader(desc.gs);
            insert_shader(desc.fs);

            std::vector<VkDescriptorSetLayout> layouts;
            VkDescriptorSetLayoutCreateInfo dslci = {};
            dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            dslci.pBindings = internal_pipeline->layout_bindings.data();
            dslci.bindingCount = static_cast<uint32_t>(internal_pipeline->layout_bindings.size());
            VK_ASSERT(vkCreateDescriptorSetLayout(device, &dslci, nullptr, &internal_pipeline->descriptor_set_layout));
            layouts.push_back(internal_pipeline->descriptor_set_layout);

            VkPipelineLayoutCreateInfo plci = {};
            plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            plci.pSetLayouts = layouts.data();
            plci.setLayoutCount = (uint32_t)layouts.size();
            if (internal_pipeline->pushconstants.size > 0) {
                plci.pushConstantRangeCount = 1;
                plci.pPushConstantRanges = &internal_pipeline->pushconstants;
            } else {
                plci.pushConstantRangeCount = 0;
                plci.pPushConstantRanges = nullptr;
            }
            VK_ASSERT(vkCreatePipelineLayout(device, &plci, nullptr, &internal_pipeline->pipeline_layout));
        }

        VkGraphicsPipelineCreateInfo& pipeline_info = internal_pipeline->pipeline_info;
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.layout = internal_pipeline->pipeline_layout;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        // shaders
        uint32_t shader_stage_count = 0;
        auto& shader_stages = internal_pipeline->shader_stages;
        if (desc.vs != nullptr) {
            VulkanShader* internal_shader = (VulkanShader*)desc.vs;
            shader_stages[shader_stage_count++] = internal_shader->stage_info;
        }
        if (desc.hs != nullptr) {
            VulkanShader* internal_shader = (VulkanShader*)desc.hs;
            shader_stages[shader_stage_count++] = internal_shader->stage_info;
        }
        if (desc.ds != nullptr) {
            VulkanShader* internal_shader = (VulkanShader*)desc.ds;
            shader_stages[shader_stage_count++] = internal_shader->stage_info;
        }
        if (desc.gs != nullptr) {
            VulkanShader* internal_shader = (VulkanShader*)desc.gs;
            shader_stages[shader_stage_count++] = internal_shader->stage_info;
        }
        if (desc.fs != nullptr) {
            VulkanShader* internal_shader = (VulkanShader*)desc.fs;
            shader_stages[shader_stage_count++] = internal_shader->stage_info;
        }
        pipeline_info.stageCount = shader_stage_count;
        pipeline_info.pStages = shader_stages;

        // input assembly
        VkPipelineInputAssemblyStateCreateInfo& input_assembly = internal_pipeline->input_assembly;
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        switch (desc.primitive_topo) {
            case PRIMITIVE_TOPO_POINT_LIST: input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
            case PRIMITIVE_TOPO_LINE_LIST: input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
            case PRIMITIVE_TOPO_LINE_STRIP: input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
            case PRIMITIVE_TOPO_TRI_STRIP: input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
            case PRIMITIVE_TOPO_PATCH_LIST: input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; break;
            case PRIMITIVE_TOPO_TRI_LIST: input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        }
        input_assembly.primitiveRestartEnable = VK_FALSE;
        pipeline_info.pInputAssemblyState = &input_assembly;

        // rasterizer
        VkPipelineRasterizationStateCreateInfo& rasterizer = internal_pipeline->rasterizer;
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_TRUE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;

        if (desc.rs != nullptr) {
            const GfxRasterizerState& rs = *desc.rs;

            switch (rs.fill_mode) {
                case FILL_WIREFRAME:
                    rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
                    break;
                case FILL_SOLID:
                default:
                    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
                    break;
            }

            switch (rs.cull_mode) {
                case CULL_BACK:
                    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
                    break;
                case CULL_FRONT:
                    rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
                    break;
                case CULL_NONE:
                default:
                    rasterizer.cullMode = VK_CULL_MODE_NONE;
                    break;
            }

            switch (rs.front_face) {
                case FRONT_FACE_CCW:
                    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                    break;
                case FRONT_FACE_CW:
                default:
                    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
                    break;
            }
            rasterizer.depthBiasEnable = rs.depth_bias != 0 || rs.slope_scaled_depth_bias != 0;
            rasterizer.depthBiasConstantFactor = static_cast<float>(rs.depth_bias);
            rasterizer.depthBiasClamp = 0.0f;
            rasterizer.depthBiasSlopeFactor = rs.slope_scaled_depth_bias;
        }
        pipeline_info.pRasterizationState = &rasterizer;

        // viewport state
        VkPipelineViewportStateCreateInfo& viewport_state = internal_pipeline->viewport_state;
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = nullptr;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = nullptr;
        pipeline_info.pViewportState = &viewport_state;

        // depth stencil
        VkPipelineDepthStencilStateCreateInfo& depthstencil = internal_pipeline->depthstencil;
        depthstencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        if (desc.dss != nullptr) {
            depthstencil.depthTestEnable = desc.dss->depth_test ? VK_TRUE : VK_FALSE;
            depthstencil.depthWriteEnable = desc.dss->depth_write ? VK_TRUE : VK_FALSE;
            depthstencil.depthCompareOp =  ToVulkanCompareOp(desc.dss->depth_func);

            depthstencil.stencilTestEnable = desc.dss->stencil_test ? VK_TRUE : VK_FALSE;

            depthstencil.front.compareMask = desc.dss->stencil_read_mask;
            depthstencil.front.writeMask = desc.dss->stencil_write_mask;
            depthstencil.front.reference = 0;
            depthstencil.front.compareOp = ToVulkanCompareOp(desc.dss->front_face.stencil_func);
            depthstencil.front.passOp = ToVulkanStencilOp(desc.dss->front_face.stencil_pass_op);
            depthstencil.front.failOp = ToVulkanStencilOp(desc.dss->front_face.stencil_fail_op);
            depthstencil.front.depthFailOp = ToVulkanStencilOp(desc.dss->front_face.stencil_depth_fail_op);

            depthstencil.back.compareMask = desc.dss->stencil_read_mask;
            depthstencil.back.writeMask = desc.dss->stencil_write_mask;
            depthstencil.back.reference = 0;
            depthstencil.back.compareOp = ToVulkanCompareOp(desc.dss->back_face.stencil_func);
            depthstencil.back.passOp = ToVulkanStencilOp(desc.dss->back_face.stencil_pass_op);
            depthstencil.back.failOp = ToVulkanStencilOp(desc.dss->back_face.stencil_fail_op);
            depthstencil.back.depthFailOp = ToVulkanStencilOp(desc.dss->back_face.stencil_depth_fail_op);

            depthstencil.depthBoundsTestEnable = VK_FALSE;
        }
        pipeline_info.pDepthStencilState = &depthstencil;

        // tessellation
        VkPipelineTessellationStateCreateInfo& tessellation_info = internal_pipeline->tessellation_state;
        tessellation_info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellation_info.patchControlPoints = desc.patch_control_points;
        pipeline_info.pTessellationState = &tessellation_info;

        VkDynamicState dynamic_states[6];
        dynamic_states[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamic_states[1] = VK_DYNAMIC_STATE_SCISSOR;
        dynamic_states[2] = VK_DYNAMIC_STATE_DEPTH_BIAS;
        dynamic_states[3] = VK_DYNAMIC_STATE_BLEND_CONSTANTS;
        dynamic_states[4] = VK_DYNAMIC_STATE_DEPTH_BOUNDS;
        //dynamic_states[5] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;

        VkPipelineDynamicStateCreateInfo dynamic_state = {};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.pNext = NULL;
        dynamic_state.flags = 0;
        dynamic_state.dynamicStateCount = 5;
        dynamic_state.pDynamicStates = dynamic_states;
        pipeline_info.pDynamicState = &dynamic_state;

        if (desc.sc) {
            pipeline_info.renderPass = ((VulkanSwapChain*)desc.sc)->renderpass;
            pipeline_info.subpass = 0;
        } else {
            pipeline_info.renderPass = ((VulkanRenderPass*)desc.rp)->renderpass;
            pipeline_info.subpass = 0;
        }

        // MSAA
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        if (desc.rs != nullptr) {
            multisampling.rasterizationSamples = ToVulkanSampleCount(desc.sample_count);
        }
        if (desc.bs != nullptr) {
            multisampling.alphaToCoverageEnable = desc.bs->alpha_to_coverage_eEnable ? VK_TRUE : VK_FALSE;
        }
        else {
            multisampling.alphaToCoverageEnable = VK_FALSE;
        }
        multisampling.alphaToOneEnable = VK_FALSE;
        pipeline_info.pMultisampleState = &multisampling;

        // Blending
        uint32_t render_target_count = 0;
        if (desc.sc) {
            render_target_count = 1;
        } else {
            for (uint32_t i = 0; i < desc.rp->desc.attachments.size(); ++i) {
                if (desc.rp->desc.attachments[i].type == RenderPassAttachment::RENDERTARGET) {
                    render_target_count++;
                }
            }
        }

        uint32_t num_blend_attachments = 0;
        VkPipelineColorBlendAttachmentState color_blend_attachments[8] = {};
        for (uint32_t i = 0; i < render_target_count; ++i) {
            size_t attachment_index = 0;
            if (desc.bs->independent_blend_enable)
                attachment_index = i;

            const auto& rt_desc = desc.bs->rt[attachment_index];
            VkPipelineColorBlendAttachmentState& attachment = color_blend_attachments[num_blend_attachments];
            num_blend_attachments++;

            attachment.blendEnable = rt_desc.blend_enable ? VK_TRUE : VK_FALSE;

            attachment.colorWriteMask = 0;
            if (rt_desc.render_target_write_mask & COLOR_WRITE_ENABLE_RED) {
                attachment.colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
            }
            if (rt_desc.render_target_write_mask & COLOR_WRITE_ENABLE_GREEN) {
                attachment.colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
            }
            if (rt_desc.render_target_write_mask & COLOR_WRITE_ENABLE_BLUE) {
                attachment.colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
            }
            if (rt_desc.render_target_write_mask & COLOR_WRITE_ENABLE_ALPHA) {
                attachment.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
            }

            attachment.srcColorBlendFactor = ToVulkanBlendFactor(rt_desc.src_factor);
            attachment.dstColorBlendFactor = ToVulkanBlendFactor(rt_desc.dst_factor);
            attachment.colorBlendOp = ToVulkanBlendOp(rt_desc.blend_op);
            attachment.srcAlphaBlendFactor = ToVulkanBlendFactor(rt_desc.src_factor_alpha);
            attachment.dstAlphaBlendFactor = ToVulkanBlendFactor(rt_desc.dst_factor_alpha);
            attachment.alphaBlendOp = ToVulkanBlendOp(rt_desc.blend_op_alpha);
        }

        VkPipelineColorBlendStateCreateInfo color_blending_info = {};
        color_blending_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending_info.logicOpEnable = VK_FALSE;
        color_blending_info.logicOp = VK_LOGIC_OP_COPY;
        color_blending_info.attachmentCount = num_blend_attachments;
        color_blending_info.pAttachments = color_blend_attachments;
        color_blending_info.blendConstants[0] = 1.0f;
        color_blending_info.blendConstants[1] = 1.0f;
        color_blending_info.blendConstants[2] = 1.0f;
        color_blending_info.blendConstants[3] = 1.0f;
        pipeline_info.pColorBlendState = &color_blending_info;

        // InputLayout
        uint32_t num_input_bindings = 0;
        VkVertexInputBindingDescription input_bindings[MAX_VERTEX_BINDINGS] = { { 0 } };
        uint32_t num_input_attributes = 0;
        VkVertexInputAttributeDescription input_attributes[MAX_VERTEX_ATTRIBS] = { { 0 } };
        uint32_t binding_value = UINT32_MAX;
        for (auto& element : desc.il->elements) {
            if (binding_value != element.binding) {
                binding_value = element.binding;
                ++num_input_bindings;
            }

            input_bindings[num_input_bindings - 1].binding = binding_value;
            if (element.rate == VERTEX_ATTRIB_RATE_INSTANCE) {
                input_bindings[num_input_bindings - 1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            } else {
                input_bindings[num_input_bindings - 1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }
            input_bindings[num_input_bindings - 1].stride += GetFormatStride(element.format);

            input_attributes[num_input_attributes].location = element.location;
            input_attributes[num_input_attributes].binding = element.binding;
            input_attributes[num_input_attributes].format = ToVulkanFormat(element.format);
            input_attributes[num_input_attributes].offset = element.offset;
            ++num_input_attributes;
        }

        VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = num_input_bindings;
        vertex_input_info.pVertexBindingDescriptions = input_bindings;
        vertex_input_info.vertexAttributeDescriptionCount = num_input_attributes;
        vertex_input_info.pVertexAttributeDescriptions = input_attributes;
        pipeline_info.pVertexInputState = &vertex_input_info;

        VK_ASSERT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &internal_pipeline->pipeline));

        return internal_pipeline;
    }

    void VulkanDevice::DestroyPipeline(GfxPipeline* pipeline) {
        resource_manager.destroy_locker.lock();
        VulkanPipeline* internal_pipeline = (VulkanPipeline*)pipeline;
        uint64_t frame_count = resource_manager.frame_count;
        resource_manager.destroyer_pipelines.push_back(std::make_pair(internal_pipeline->pipeline, frame_count));
        resource_manager.destroyer_pipeline_layouts.push_back(std::make_pair(internal_pipeline->pipeline_layout, frame_count));
        resource_manager.destroyer_descriptor_set_layouts.push_back(std::make_pair(internal_pipeline->descriptor_set_layout, frame_count));
        BLAST_SAFE_DELETE(pipeline);
        resource_manager.destroy_locker.unlock();
    }

    GfxCommandBuffer* VulkanDevice::RequestCommandBuffer(QueueType type) {
        if (type == QUEUE_COPY) {
            VulkanCommandBuffer* internal_cmd = nullptr;
            internal_cmd = new VulkanCommandBuffer();
            internal_cmd->idx = copy_pool.Allocate();
            internal_cmd->is_copy = true;
            copy_cmds.push_back(internal_cmd);
            return internal_cmd;
        }

        VulkanCommandBuffer* internal_cmd = nullptr;
        internal_cmd = new VulkanCommandBuffer();
        internal_cmd->idx = work_cmds.size();
        work_cmds.push_back(internal_cmd);

        uint32_t cmd = internal_cmd->idx;
        cmd_meta[cmd].queue = type;
        cmd_meta[cmd].waits.clear();

        if (GetCommandBuffer(cmd) == VK_NULL_HANDLE) {
            for (auto& frame : frames) {
                VkCommandPoolCreateInfo pool_info = {};
                pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                switch (type) {
                    case QUEUE_GRAPHICS:
                        pool_info.queueFamilyIndex = graphics_family;
                        break;
                    case QUEUE_COMPUTE:
                        pool_info.queueFamilyIndex = compute_family;
                        break;
                    default:
                        assert(0);
                        break;
                }
                pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
                VK_ASSERT(vkCreateCommandPool(device, &pool_info, nullptr, &frame.command_pools[cmd][type]));

                VkCommandBufferAllocateInfo cmd_info = {};
                cmd_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                cmd_info.commandBufferCount = 1;
                cmd_info.commandPool = frame.command_pools[cmd][type];
                cmd_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                VK_ASSERT(vkAllocateCommandBuffers(device, &cmd_info, &frame.command_buffers[cmd][type]));

                frame.descriptor_pools[cmd].Init(this);

                frame.stage_buffers[cmd] = new StageBuffer();
                frame.stage_buffers[cmd]->Init(this);
            }

            binders[cmd].Init(this);
        }

        VK_ASSERT(vkResetCommandPool(device, GetFrameResources().command_pools[cmd][type], 0));

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = nullptr;
        VK_ASSERT(vkBeginCommandBuffer(GetFrameResources().command_buffers[cmd][type], &begin_info));

        // 重置Descriptor对象
        GetFrameResources().descriptor_pools[cmd].Reset();
        binders[cmd].Reset();

        if (type == QUEUE_GRAPHICS) {
            VkRect2D scissors[8];
            for (int i = 0; i < 8; ++i) {
                scissors[i].offset.x = 0;
                scissors[i].offset.y = 0;
                scissors[i].extent.width = 65535;
                scissors[i].extent.height = 65535;
            }
            vkCmdSetScissor(GetCommandBuffer(cmd), 0, 8, scissors);

            float blend_constants[] = { 1,1,1,1 };
            vkCmdSetBlendConstants(GetCommandBuffer(cmd), blend_constants);
        }

        active_pipeline[cmd] = nullptr;
        active_cs[cmd] = nullptr;
        dirty_pipeline[cmd] = false;
        pushconstants[cmd] = {};
        active_swapchains[cmd].clear();
        return internal_cmd;
    }

    void VulkanDevice::SubmitAllCommandBuffer() {
        init_locker.lock();
        {
            auto& frame = GetFrameResources();
            QueueType submit_queue = QUEUE_GRAPHICS;

            if(submit_inits) {
                vkEndCommandBuffer(frame.init_command_buffer);
            }

            uint64_t copy_sync = copy_pool.Submit();

            for (uint32_t i = 0; i < work_cmds.size(); ++i) {
                uint32_t cmd = ((VulkanCommandBuffer*)work_cmds[i])->idx;
                vkEndCommandBuffer(GetCommandBuffer(cmd));

                const CommandListMetadata& meta = cmd_meta[cmd];

                if (submit_queue == QUEUE_GRAPHICS) {
                    submit_queue = meta.queue;
                }

                if (copy_sync > 0) {
                    queues[submit_queue].submit_wait_stages.push_back(VK_PIPELINE_STAGE_TRANSFER_BIT);
                    queues[submit_queue].submit_wait_semaphores.push_back(copy_pool.semaphore);
                    queues[submit_queue].submit_wait_values.push_back(copy_sync);
                    copy_sync = 0;
                }

                if (submit_queue != meta.queue || !meta.waits.empty()) {
                    queues[submit_queue].submit_signal_semaphores.push_back(queues[submit_queue].semaphore);
                    queues[submit_queue].submit_signal_values.push_back(frame_count * BLAST_CMD_COUNT + (uint64_t)cmd);
                    queues[submit_queue].Submit(VK_NULL_HANDLE);
                    submit_queue = meta.queue;

                    for (auto& wait : meta.waits) {
                        const CommandListMetadata& wait_meta = cmd_meta[wait];
                        queues[submit_queue].submit_wait_stages.push_back(VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
                        queues[submit_queue].submit_wait_semaphores.push_back(queues[wait_meta.queue].semaphore);
                        queues[submit_queue].submit_wait_values.push_back(frame_count * BLAST_CMD_COUNT + (uint64_t)wait);
                    }
                }

                if (submit_inits) {
                    queues[submit_queue].submit_cmds.push_back(frame.init_command_buffer);
                    submit_inits = false;
                }

                for (auto& swapchain : active_swapchains[cmd]) {
                    VulkanSwapChain* internal_swapchain = (VulkanSwapChain*)swapchain;



                    queues[submit_queue].submit_swapchains.push_back(internal_swapchain->swapchain);
                    queues[submit_queue].submit_swapchain_image_indices.push_back(internal_swapchain->swapchain_image_index);
                    queues[submit_queue].submit_wait_stages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
                    queues[submit_queue].submit_wait_semaphores.push_back(internal_swapchain->swapchain_acquire_semaphore);
                    queues[submit_queue].submit_wait_values.push_back(0);
                    queues[submit_queue].submit_signal_semaphores.push_back(internal_swapchain->swapchain_release_semaphore);
                    queues[submit_queue].submit_signal_values.push_back(0);
                }

                queues[submit_queue].submit_cmds.push_back(GetCommandBuffer(cmd));
            }

            // 清理当前帧的命令缓存
            for (uint32_t i = 0; i < copy_cmds.size(); ++i) {
                BLAST_SAFE_DELETE(copy_cmds[i]);
            }
            copy_cmds.clear();

            for (uint32_t i = 0; i < work_cmds.size(); ++i) {
                BLAST_SAFE_DELETE(work_cmds[i]);
            }
            work_cmds.clear();

            for (int queue = 0; queue < BLAST_QUEUE_COUNT; ++queue) {
                queues[queue].Submit(frame.fence[queue]);
            }
        }

        frame_count++;

        {
            auto& frame = GetFrameResources();

            if (frame_count >= BLAST_BUFFER_COUNT) {
                for (int queue = 0; queue < BLAST_QUEUE_COUNT; ++queue) {
                    VK_ASSERT(vkWaitForFences(device, 1, &frame.fence[queue], true, 0xFFFFFFFFFFFFFFFF));
                    VK_ASSERT(vkResetFences(device, 1, &frame.fence[queue]));
                }
            }

            // 销毁上一帧需要清理的资源
            resource_manager.Update(frame_count, BLAST_BUFFER_COUNT);

            // 重置transfer命令缓存状态
            {
                VK_ASSERT(vkResetCommandPool(device, frame.init_command_pool, 0));

                VkCommandBufferBeginInfo begin_info = {};
                begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                begin_info.pInheritanceInfo = nullptr;
                VK_ASSERT(vkBeginCommandBuffer(frame.init_command_buffer, &begin_info));
            }
        }

        submit_inits = false;
        init_locker.unlock();
    }

    void VulkanDevice::WaitCommandBuffer(GfxCommandBuffer* cmd, GfxCommandBuffer* wait_for) {
        VulkanCommandBuffer* internal_cmd = (VulkanCommandBuffer*)cmd;
        VulkanCommandBuffer* internal_wait_for = (VulkanCommandBuffer*)wait_for;
        cmd_meta[internal_cmd->idx].waits.push_back(internal_wait_for->idx);
    }

    void VulkanDevice::RenderPassBegin(GfxCommandBuffer* cmd, GfxSwapChain* swapchain) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        VulkanSwapChain* internal_swapchain = (VulkanSwapChain*)swapchain;

        active_swapchains[internal_cmd].push_back(swapchain);

        VK_ASSERT(vkAcquireNextImageKHR(
                device,
                internal_swapchain->swapchain,
                0xFFFFFFFFFFFFFFFF,
                internal_swapchain->swapchain_acquire_semaphore,
                VK_NULL_HANDLE,
                &internal_swapchain->swapchain_image_index
        ));

        std::vector<VkClearValue> clear_values;
        VkClearValue clear_color = {
                swapchain->desc.clear_color[0],
                swapchain->desc.clear_color[1],
                swapchain->desc.clear_color[2],
                swapchain->desc.clear_color[3],
        };
        clear_values.push_back(clear_color);

        VkRenderPassBeginInfo renderpass_info = {};
        renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderpass_info.renderPass = internal_swapchain->renderpass;
        renderpass_info.framebuffer = internal_swapchain->swapchain_framebuffers[internal_swapchain->swapchain_image_index];
        renderpass_info.renderArea.offset = { 0, 0 };
        renderpass_info.renderArea.extent = internal_swapchain->swapchain_extent;
        renderpass_info.clearValueCount = clear_values.size();
        renderpass_info.pClearValues = clear_values.data();
        vkCmdBeginRenderPass(GetCommandBuffer(internal_cmd), &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanDevice::RenderPassBegin(GfxCommandBuffer* cmd, GfxRenderPass* renderpass) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        VulkanRenderPass* internal_renderpass = (VulkanRenderPass*)renderpass;

        vkCmdBeginRenderPass(GetCommandBuffer(internal_cmd), &internal_renderpass->begin_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanDevice::RenderPassEnd(GfxCommandBuffer* cmd) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        vkCmdEndRenderPass(GetCommandBuffer(internal_cmd));
    }

    void VulkanDevice::BindScissorRects(GfxCommandBuffer* cmd, uint32_t num_rects, Rect* rects) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        VkRect2D scissors[16];
        for(uint32_t i = 0; i < num_rects; ++i) {
            scissors[i].extent.width = abs(rects[i].right - rects[i].left);
            scissors[i].extent.height = abs(rects[i].top - rects[i].bottom);
            scissors[i].offset.x = std::max(0, rects[i].left);
            scissors[i].offset.y = std::max(0, rects[i].top);
        }
        vkCmdSetScissor(GetCommandBuffer(internal_cmd), 0, num_rects, scissors);
    }

    void VulkanDevice::BindViewports(GfxCommandBuffer* cmd, uint32_t num_viewports, Viewport* viewports) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        VkViewport vp[16];
        for (uint32_t i = 0; i < num_viewports; ++i) {
            vp[i].x = viewports[i].x;
            vp[i].y = viewports[i].y;
            vp[i].width = viewports[i].w;
            vp[i].height = viewports[i].h;
            vp[i].minDepth = viewports[i].min_depth;
            vp[i].maxDepth = viewports[i].max_depth;
        }
        vkCmdSetViewport(GetCommandBuffer(internal_cmd), 0, num_viewports, vp);
    }

    void VulkanDevice::BindResource(GfxCommandBuffer* cmd, GfxResource* resource, uint32_t slot, int32_t subresource) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        auto& binder = binders[internal_cmd];
        if (binder.table.srv[slot] != resource || binder.table.srv_index[slot] != subresource) {
            binder.table.srv[slot] = resource;
            binder.table.srv_index[slot] = subresource;
            binder.dirty = true;
        }
    }

    void VulkanDevice::BindResources(GfxCommandBuffer* cmd, GfxResource** resources, uint32_t slot, uint32_t count) {
        if (resources != nullptr) {
            for (uint32_t i = 0; i < count; ++i) {
                BindResource(cmd, resources[i], slot + i, -1);
            }
        }
    }

    void VulkanDevice::BindUAV(GfxCommandBuffer* cmd, GfxResource* resource, uint32_t slot, int32_t subresource) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        auto& binder = binders[internal_cmd];
        if (binder.table.uav[slot] != resource || binder.table.uav_index[slot] != subresource) {
            binder.table.uav[slot] = resource;
            binder.table.uav_index[slot] = subresource;
            binder.dirty = true;
        }
    }

    void VulkanDevice::BindUAVs(GfxCommandBuffer* cmd, GfxResource** resources, uint32_t slot, uint32_t count) {
        if (resources != nullptr) {
            for (uint32_t i = 0; i < count; ++i) {
                BindUAV(cmd, resources[i], slot + i, -1);
            }
        }
    }

    void VulkanDevice::BindSampler(GfxCommandBuffer* cmd, GfxSampler* sampler, uint32_t slot) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        auto& binder = binders[internal_cmd];
        if (binder.table.sam[slot] != sampler) {
            binder.table.sam[slot] = sampler;
            binder.dirty = true;
        }
    };

    void VulkanDevice::BindConstantBuffer(GfxCommandBuffer* cmd, GfxBuffer* buffer, uint32_t slot, uint64_t size, uint64_t offset) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        auto& binder = binders[internal_cmd];
        if (binder.table.cbv[slot] != buffer || binder.table.cbv_offset[slot] != offset || binder.table.cbv_size[slot] != size) {
            binder.table.cbv[slot] = buffer;
            binder.table.cbv_offset[slot] = offset;
            binder.table.cbv_size[slot] = size;
            binder.dirty = true;
        }
    }

    void VulkanDevice::BindVertexBuffers(GfxCommandBuffer* cmd, GfxBuffer** vertex_buffers, uint32_t slot, uint32_t count, uint64_t* offsets) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;

        VkDeviceSize voffsets[8] = {};
        VkBuffer vbuffers[8] = {};
        for (uint32_t i = 0; i < count; ++i) {
            VulkanBuffer* internal_buffer = (VulkanBuffer*)vertex_buffers[i];
            vbuffers[i] = internal_buffer->resource;
            voffsets[i] = offsets[i];
        }

        vkCmdBindVertexBuffers(GetCommandBuffer(internal_cmd), static_cast<uint32_t>(slot), static_cast<uint32_t>(count), vbuffers, voffsets);
    }

    void VulkanDevice::BindIndexBuffer(GfxCommandBuffer* cmd, GfxBuffer* index_buffer, IndexType type, uint64_t offset) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        if (index_buffer != nullptr) {
            VulkanBuffer* internal_buffer = (VulkanBuffer*)index_buffer;
            vkCmdBindIndexBuffer(GetCommandBuffer(internal_cmd), internal_buffer->resource, offset, ToVulkanIndexType(type));
        }
    }

    void VulkanDevice::BindStencilRef(GfxCommandBuffer* cmd, uint32_t value) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        vkCmdSetStencilReference(GetCommandBuffer(internal_cmd), VK_STENCIL_FRONT_AND_BACK, value);
    }

    void VulkanDevice::BindBlendFactor(GfxCommandBuffer* cmd, float r, float g, float b, float a) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        float blend_constants[] = { r, g, b, a };
        vkCmdSetBlendConstants(GetCommandBuffer(internal_cmd), blend_constants);
    }

    void VulkanDevice::BindPipeline(GfxCommandBuffer* cmd, GfxPipeline* pipeline) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        VulkanPipeline* internal_pipeline = (VulkanPipeline*)pipeline;
        if (!active_pipeline[internal_cmd]) {
            active_pipeline[internal_cmd] = pipeline;
            dirty_pipeline[internal_cmd] = true;
            binders[internal_cmd].dirty = true;
        } else {
            VulkanPipeline* internal_active_pipeline = (VulkanPipeline*)active_pipeline[internal_cmd];
            if (internal_active_pipeline->hash != internal_pipeline->hash) {
                active_pipeline[internal_cmd] = pipeline;
                dirty_pipeline[internal_cmd] = true;
                binders[internal_cmd].dirty = true;
            }
        }
    }

    void VulkanDevice::BindComputeShader(GfxCommandBuffer* cmd, GfxShader* cs) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        if (active_cs[internal_cmd] != cs) {
            binders[internal_cmd].dirty = true;
            active_cs[internal_cmd] = cs;
            VulkanShader* internal_shader = (VulkanShader*)cs;
            vkCmdBindPipeline(GetCommandBuffer(internal_cmd), VK_PIPELINE_BIND_POINT_COMPUTE, internal_shader->pipeline_cs);
        }
    }

    void VulkanDevice::PushConstants(GfxCommandBuffer* cmd, const void* data, uint32_t size) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        memcpy(pushconstants[internal_cmd].data, data, size);
        pushconstants[internal_cmd].size = size;
    }

    void VulkanDevice::PipelineStateValidate(uint32_t cmd) {
        if (!dirty_pipeline[cmd])
            return;

        VulkanPipeline* internal_pipeline = (VulkanPipeline*)active_pipeline[cmd];
        vkCmdBindPipeline(GetCommandBuffer(cmd), VK_PIPELINE_BIND_POINT_GRAPHICS, internal_pipeline->pipeline);
    }

    void VulkanDevice::PreDraw(uint32_t cmd) {
        PipelineStateValidate(cmd);

        binders[cmd].Flush(true, cmd);

        VulkanPipeline* internal_pipeline = (VulkanPipeline*)active_pipeline[cmd];
        if (internal_pipeline->pushconstants.size > 0) {
            vkCmdPushConstants(
                    GetCommandBuffer(cmd),
                    internal_pipeline->pipeline_layout,
                    internal_pipeline->pushconstants.stageFlags,
                    internal_pipeline->pushconstants.offset,
                    internal_pipeline->pushconstants.size,
                    pushconstants[cmd].data
            );
        }
    }

    void VulkanDevice::PreDispatch(uint32_t cmd) {
        binders[cmd].Flush(false, cmd);

        VulkanShader* internal_cs = (VulkanShader*)active_cs[cmd];
        if (internal_cs->pushconstants.size > 0) {
            vkCmdPushConstants(
                    GetCommandBuffer(cmd),
                    internal_cs->pipeline_layout_cs,
                    internal_cs->pushconstants.stageFlags,
                    internal_cs->pushconstants.offset,
                    internal_cs->pushconstants.size,
                    pushconstants[cmd].data
            );
        }
    }

    void VulkanDevice::Draw(GfxCommandBuffer* cmd, uint32_t vertex_count, uint32_t vertex_offset) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        PreDraw(internal_cmd);
        vkCmdDraw(GetCommandBuffer(internal_cmd), vertex_count, 1, vertex_offset, 0);
    }

    void VulkanDevice::DrawIndexed(GfxCommandBuffer* cmd, uint32_t index_count, uint32_t index_offset, int32_t vertex_offset) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        PreDraw(internal_cmd);
        vkCmdDrawIndexed(GetCommandBuffer(internal_cmd), index_count, 1, index_offset, vertex_offset, 0);
    }

    void VulkanDevice::DrawInstanced(GfxCommandBuffer* cmd, uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        PreDraw(internal_cmd);
        vkCmdDraw(GetCommandBuffer(internal_cmd), vertex_count, instance_count, vertex_offset, instance_offset);
    }

    void VulkanDevice::DrawIndexedInstanced(GfxCommandBuffer* cmd, uint32_t index_count, uint32_t instance_count, uint32_t index_offset, int32_t vertex_offset, uint32_t instance_offset) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        PreDraw(internal_cmd);
        vkCmdDrawIndexed(GetCommandBuffer(internal_cmd), index_count, instance_count, index_offset, vertex_offset, instance_offset);
    }

    void VulkanDevice::Dispatch(GfxCommandBuffer* cmd, uint32_t thread_group_x, uint32_t thread_group_y, uint32_t thread_group_z) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        PreDispatch(internal_cmd);
        vkCmdDispatch(GetCommandBuffer(internal_cmd), thread_group_x, thread_group_y, thread_group_z);
    }

    void VulkanDevice::BufferCopy(GfxCommandBuffer* cmd, const GfxBufferCopyRange& range) {
        VulkanBuffer* internel_src_buffer = static_cast<VulkanBuffer*>(range.src_buffer);
        VulkanBuffer* internel_dst_buffer = static_cast<VulkanBuffer*>(range.dst_buffer);
        VkBufferCopy copy = {};
        copy.srcOffset = range.src_offset;
        copy.dstOffset = range.dst_offset;
        copy.size = range.size;

        VkCommandBuffer command_buffer;
        if (((VulkanCommandBuffer*)cmd)->is_copy) {
            command_buffer = GetCopyCommandBuffer(((VulkanCommandBuffer*)cmd)->idx).command_buffer;
        } else {
            command_buffer = GetCommandBuffer(((VulkanCommandBuffer*)cmd)->idx);
        }

        vkCmdCopyBuffer(command_buffer, internel_src_buffer->resource, internel_dst_buffer->resource, 1, &copy);
    }

    void VulkanDevice::ImageCopy(GfxCommandBuffer* cmd, const GfxImageCopyRange& range) {
        VulkanTexture* internel_src_texture = static_cast<VulkanTexture*>(range.src_texture);
        VulkanTexture* internel_dst_texture = static_cast<VulkanTexture*>(range.dst_texture);
        VkImageCopy copy = {};
        copy.extent.width = range.width;
        copy.extent.height = range.height;
        copy.extent.depth = range.depth;

        copy.srcSubresource.aspectMask = ToVulkanAspectMask(internel_src_texture->desc.format);
        copy.srcSubresource.baseArrayLayer = range.src_layer;
        copy.srcSubresource.mipLevel = range.src_level;
        copy.srcSubresource.layerCount = 1;
        copy.srcOffset = { 0, 0, 0 };

        copy.dstSubresource.aspectMask = ToVulkanAspectMask(internel_dst_texture->desc.format);
        copy.dstSubresource.baseArrayLayer = range.dst_layer;
        copy.dstSubresource.mipLevel = range.dst_level;
        copy.dstSubresource.layerCount = 1;
        copy.dstOffset = { 0, 0, 0 };

        VkCommandBuffer command_buffer;
        if (((VulkanCommandBuffer*)cmd)->is_copy) {
            command_buffer = GetCopyCommandBuffer(((VulkanCommandBuffer*)cmd)->idx).command_buffer;
        } else {
            command_buffer = GetCommandBuffer(((VulkanCommandBuffer*)cmd)->idx);
        }

        vkCmdCopyImage(command_buffer, internel_src_texture->resource, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, internel_dst_texture->resource, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    }

    void VulkanDevice::BufferImageCopy(GfxCommandBuffer* cmd, const GfxBufferImageCopyRange& range) {
        VulkanBuffer* internel_src_buffer = static_cast<VulkanBuffer*>(range.src_buffer);
        VulkanTexture* internel_dst_texture = static_cast<VulkanTexture*>(range.dst_texture);
        VkBufferImageCopy copy = {};
        copy.bufferOffset = 0;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;
        copy.imageSubresource.aspectMask = ToVulkanAspectMask(internel_dst_texture->desc.format);
        copy.imageSubresource.mipLevel = range.level;
        copy.imageSubresource.baseArrayLayer = range.layer;
        copy.imageSubresource.layerCount = 1;
        copy.imageOffset.x = 0;
        copy.imageOffset.y = 0;
        copy.imageOffset.z = 0;
        copy.imageExtent.width = internel_dst_texture->desc.width;
        copy.imageExtent.height = internel_dst_texture->desc.height;
        copy.imageExtent.depth = internel_dst_texture->desc.depth;

        VkCommandBuffer command_buffer;
        if (((VulkanCommandBuffer*)cmd)->is_copy) {
            command_buffer = GetCopyCommandBuffer(((VulkanCommandBuffer*)cmd)->idx).command_buffer;
        } else {
            command_buffer = GetCommandBuffer(((VulkanCommandBuffer*)cmd)->idx);
        }

        vkCmdCopyBufferToImage(command_buffer, internel_src_buffer->resource, internel_dst_texture->resource, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    }

    void VulkanDevice::UpdateBuffer(GfxCommandBuffer* cmd, GfxBuffer* buffer, const void* data, uint64_t size, uint64_t offset) {
        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        StageBuffer::Allocation allocation = {};
        if (((VulkanCommandBuffer*)cmd)->is_copy) {
            allocation = GetCopyCommandBuffer(internal_cmd).stage_buffer->Allocate(size);
        } else {
            allocation = GetFrameResources().stage_buffers[internal_cmd]->Allocate(size);
        }

        {
            void* dst_data;
            VulkanBuffer* internal_buffer = (VulkanBuffer*)allocation.buffer;
            vkMapMemory(device, internal_buffer->memory, allocation.offset, size, 0, &dst_data);
            memcpy(dst_data, data, static_cast<size_t>(size));
            vkUnmapMemory(device, internal_buffer->memory);
        }

        GfxBufferCopyRange copy_range;
        copy_range.size = size;
        copy_range.src_buffer = allocation.buffer;
        copy_range.src_offset = allocation.offset;
        copy_range.dst_buffer = buffer;
        copy_range.dst_offset = offset;
        BufferCopy(cmd, copy_range);
    }

    void VulkanDevice::UpdateTexture(GfxCommandBuffer* cmd, GfxTexture* texture, const void* data, uint32_t layer, uint32_t level) {
        uint32_t total_image_size = 0;
        for (uint32_t i = 0; i < layer + 1; ++i) {
            for (uint32_t j = 0; j < level + 1; ++j) {
                uint32_t image_size = 0;
                if (texture->desc.depth > 1) {
                    image_size = texture->desc.width >> j;
                    image_size *= texture->desc.height >> j;
                    image_size *= texture->desc.depth >> j;
                    image_size *= blast::GetFormatStride(texture->desc.format);
                } else if (texture->desc.height > 1) {
                    image_size = texture->desc.width >> j;
                    image_size *= texture->desc.height >> j;
                    image_size *= blast::GetFormatStride(texture->desc.format);
                } else {
                    image_size = texture->desc.width >> j;
                    image_size *= blast::GetFormatStride(texture->desc.format);
                }
                total_image_size += image_size;
            }
        }

        uint32_t internal_cmd = ((VulkanCommandBuffer*)cmd)->idx;
        StageBuffer::Allocation allocation = {};
        if (((VulkanCommandBuffer*)cmd)->is_copy) {
            allocation = GetCopyCommandBuffer(internal_cmd).stage_buffer->Allocate(total_image_size);
        } else {
            allocation = GetFrameResources().stage_buffers[internal_cmd]->Allocate(total_image_size);
        }

        {
            void* dst_data;
            VulkanBuffer* internal_buffer = (VulkanBuffer*)allocation.buffer;
            vkMapMemory(device, internal_buffer->memory, allocation.offset, VK_WHOLE_SIZE, 0, &dst_data);
            memcpy(dst_data, data, static_cast<size_t>(total_image_size));
            vkUnmapMemory(device, internal_buffer->memory);
        }

        for (uint32_t i = 0; i < layer + 1; ++i) {
            for (uint32_t j = 0; j < level + 1; ++j) {
                GfxBufferImageCopyRange copy_range;
                copy_range.src_buffer = allocation.buffer;
                copy_range.dst_texture = texture;
                copy_range.layer = i;
                copy_range.level = j;
                BufferImageCopy(cmd, copy_range);
            }
        }
    }

    void VulkanDevice::SetBarrier(GfxCommandBuffer* cmd, uint32_t num_buffer_barriers, GfxBufferBarrier* buffer_barriers,
                    uint32_t num_texture_barriers, GfxTextureBarrier* texture_barriers) {
        std::vector<VkImageMemoryBarrier> internel_image_barriers;
        std::vector<VkBufferMemoryBarrier> internel_buffer_barriers;

        VkAccessFlags src_access_flags = 0;
        VkAccessFlags dst_access_flags = 0;

        for (uint32_t i = 0; i < num_buffer_barriers; ++i) {
            GfxBufferBarrier* trans = &buffer_barriers[i];
            VulkanBuffer* buffer = static_cast<VulkanBuffer*>(trans->buffer);
            bool flag = false;
            VkBufferMemoryBarrier buffer_barrier = {};
            if (!(trans->new_state & buffer->res_state)) {
                flag = true;
                buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                buffer_barrier.pNext = NULL;
                buffer_barrier.srcAccessMask = ToVulkanAccessFlags(buffer->res_state);
                buffer_barrier.dstAccessMask = ToVulkanAccessFlags(trans->new_state);

                buffer->res_state = trans->new_state;
            }
            else if(trans->new_state == RESOURCE_STATE_UNORDERED_ACCESS) {
                flag = true;
                buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                buffer_barrier.pNext = NULL;
                buffer_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
                buffer_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
            }

            if(flag) {
                buffer_barrier.buffer = buffer->resource;
                buffer_barrier.size = VK_WHOLE_SIZE;
                buffer_barrier.offset = 0;
                buffer_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                buffer_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                src_access_flags |= buffer_barrier.srcAccessMask;
                dst_access_flags |= buffer_barrier.dstAccessMask;
                internel_buffer_barriers.push_back(buffer_barrier);
            }
        }

        for (uint32_t i = 0; i < num_texture_barriers; ++i) {
            GfxTextureBarrier* trans = &texture_barriers[i];
            VulkanTexture* texture = static_cast<VulkanTexture*>(trans->texture);
            bool flag = false;
            VkImageMemoryBarrier image_barrier = {};
            if (!(trans->new_state & texture->res_state)) {
                flag = true;
                image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                image_barrier.pNext = NULL;
                image_barrier.srcAccessMask = ToVulkanAccessFlags(texture->res_state);
                image_barrier.dstAccessMask = ToVulkanAccessFlags(trans->new_state);
                image_barrier.oldLayout = ToVulkanImageLayout(texture->res_state);
                image_barrier.newLayout = ToVulkanImageLayout(trans->new_state);

                texture->res_state = trans->new_state;
            }
            else if(trans->new_state == RESOURCE_STATE_UNORDERED_ACCESS) {
                flag = true;
                image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                image_barrier.pNext = NULL;
                image_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
                image_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
                image_barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
                image_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            }

            if(flag) {
                image_barrier.image = texture->resource;
                image_barrier.subresourceRange.aspectMask = ToVulkanAspectMask(texture->desc.format);
                image_barrier.subresourceRange.baseMipLevel = 0;
                image_barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
                image_barrier.subresourceRange.baseArrayLayer = 0;
                image_barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
                image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                src_access_flags |= image_barrier.srcAccessMask;
                dst_access_flags |= image_barrier.dstAccessMask;
                internel_image_barriers.push_back(image_barrier);
            }
        }

        QueueType queue_type;
        VkCommandBuffer command_buffer;
        if (((VulkanCommandBuffer*)cmd)->is_copy) {
            queue_type = QUEUE_COPY;
            command_buffer = GetCopyCommandBuffer(((VulkanCommandBuffer*)cmd)->idx).command_buffer;
        } else {
            queue_type = cmd_meta[((VulkanCommandBuffer*)cmd)->idx].queue;
            command_buffer = GetCommandBuffer(((VulkanCommandBuffer*)cmd)->idx);
        }

        VkPipelineStageFlags src_stage_mask = ToPipelineStageFlags(src_access_flags, queue_type);
        VkPipelineStageFlags dst_stage_mask = ToPipelineStageFlags(dst_access_flags, queue_type);

        if (internel_image_barriers.size() || internel_buffer_barriers.size()) {
            vkCmdPipelineBarrier(command_buffer, src_stage_mask, dst_stage_mask, 0, 0, NULL,
                                 internel_buffer_barriers.size(), internel_buffer_barriers.data(),
                                 internel_image_barriers.size(), internel_image_barriers.data());
        }
    }
}
