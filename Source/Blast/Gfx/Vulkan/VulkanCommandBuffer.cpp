#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "VulkanFramebuffer.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanPipeline.h"

namespace blast {
    VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanContext* context, const GfxCommandBufferPoolDesc& desc)
        :GfxCommandBufferPool(desc) {
        _context = context;
        VulkanQueue* internel_queue = static_cast<VulkanQueue*>(desc.queue);
        _queue_type = internel_queue->GetType();
        
        VkCommandPoolCreateInfo cpci = {};
        cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cpci.queueFamilyIndex = internel_queue->GetFamilyIndex();
        if (!desc.transient)
            cpci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        else
            cpci.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        VK_ASSERT(vkCreateCommandPool(_context->GetDevice(), &cpci, nullptr, &_pool));
    }

    VulkanCommandBufferPool::~VulkanCommandBufferPool() {
        vkDestroyCommandPool(_context->GetDevice(), _pool, nullptr);
    }

    GfxCommandBuffer* VulkanCommandBufferPool::AllocBuffer(bool secondary) {
        GfxCommandBufferDesc cmd_buf_desc;
        cmd_buf_desc.pool = this;
        cmd_buf_desc.secondary = secondary;
        return new VulkanCommandBuffer(_context, cmd_buf_desc);
    }
    
    void VulkanCommandBufferPool::DeleteBuffer(GfxCommandBuffer* buffer) {
        BLAST_SAFE_DELETE(buffer);
    }

    VulkanCommandBuffer::VulkanCommandBuffer(VulkanContext* context, const GfxCommandBufferDesc& desc)
        :GfxCommandBuffer(desc) {
        _context = context;
        _pool = static_cast<VulkanCommandBufferPool*>(desc.pool);

        VkCommandBufferAllocateInfo cbai = {};
        cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cbai.commandPool = _pool->GetHandle();
        cbai.commandBufferCount = 1;
        if (!desc.secondary)
            cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        else
            cbai.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;

        VK_ASSERT(vkAllocateCommandBuffers(_context->GetDevice(), &cbai, &_command_buffer));
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() {
        vkFreeCommandBuffers(_context->GetDevice(), _pool->GetHandle(), 1, &_command_buffer);
    }

    void VulkanCommandBuffer::Begin() {
        VkCommandBufferBeginInfo cbbi = {};
        cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cbbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_ASSERT(vkBeginCommandBuffer(_command_buffer, &cbbi));
    }

    void VulkanCommandBuffer::End() {
        VK_ASSERT(vkEndCommandBuffer(_command_buffer));
    }

    void VulkanCommandBuffer::BindFramebuffer(GfxFramebuffer* framebuffer) {
        VulkanFramebuffer* internel_framebuffer = (VulkanFramebuffer*)framebuffer;
        VkRenderPassBeginInfo rpbi = {};
        rpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpbi.renderPass = internel_framebuffer->GetRenderPass();
        rpbi.framebuffer = internel_framebuffer->GetHandle();
        rpbi.renderArea.offset.x = 0;
        rpbi.renderArea.offset.y = 0;
        rpbi.renderArea.extent.width = internel_framebuffer->GetWidth();
        rpbi.renderArea.extent.height = internel_framebuffer->GetHeight();
        rpbi.clearValueCount = 0;
        rpbi.pClearValues = nullptr;

        vkCmdBeginRenderPass(_command_buffer, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::UnbindFramebuffer() {
        vkCmdEndRenderPass(_command_buffer);
    }

    void VulkanCommandBuffer::ClearFramebuffer(GfxFramebuffer* framebuffer, const GfxClearValue& clear_value) {
        if (clear_value.flags == CLEAR_NONE) {
            return;
        }

        VulkanFramebuffer* internel_framebuffer = (VulkanFramebuffer*)framebuffer;
        VkClearRect rect[1];
        rect[0].rect.offset.x = 0.0;
        rect[0].rect.offset.y = 0.0;
        rect[0].rect.extent.width = framebuffer->GetWidth();
        rect[0].rect.extent.height = framebuffer->GetHeight();
        rect[0].baseArrayLayer = 0;
        rect[0].layerCount     = 1;

        uint32_t num_attachments = 0;
        VkClearAttachment attachments[MAX_RENDER_TARGET_ATTACHMENTS + 1];
        if (clear_value.flags & CLEAR_COLOR) {
            for (uint32_t i = 0; i < internel_framebuffer->GetColorAttachmentCount(); ++i) {
                attachments[num_attachments].colorAttachment = num_attachments;
                attachments[num_attachments].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                attachments[num_attachments].clearValue.color.float32[0] = clear_value.color[0];
                attachments[num_attachments].clearValue.color.float32[1] = clear_value.color[1];
                attachments[num_attachments].clearValue.color.float32[2] = clear_value.color[2];
                attachments[num_attachments].clearValue.color.float32[3] = clear_value.color[3];
                ++num_attachments;
            }
        }

        if ((clear_value.flags & CLEAR_DEPTH) || (clear_value.flags & CLEAR_STENCIL)) {
            VkImageAspectFlags depth_aspect_mask = 0;
            if (clear_value.flags & CLEAR_DEPTH) {
                depth_aspect_mask |= VK_IMAGE_ASPECT_DEPTH_BIT;
            }
            if (clear_value.flags & CLEAR_STENCIL) {
                depth_aspect_mask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
            attachments[num_attachments].aspectMask = depth_aspect_mask;
            attachments[num_attachments].clearValue.depthStencil.depth   = clear_value.depth;
            attachments[num_attachments].clearValue.depthStencil.stencil = clear_value.stencil;
            ++num_attachments;
        }

        if (num_attachments > 0) {
            vkCmdClearAttachments(_command_buffer, num_attachments, attachments, 1, rect);
        }
    }

    void VulkanCommandBuffer::SetViewport(float x, float y, float w, float h) {
        VkViewport viewport = {};
        viewport.x = x;
        viewport.y = y;
        viewport.width = w;
        viewport.height = h;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(_command_buffer, 0, 1, &viewport);
    }

    void VulkanCommandBuffer::SetScissor(int x, int y, int w, int h) {
        VkRect2D scissor = {};
        scissor.offset.x = x;
        scissor.offset.y = y;
        scissor.extent.width = w;
        scissor.extent.height = h;
        vkCmdSetScissor(_command_buffer, 0, 1, &scissor);
    }

    void VulkanCommandBuffer::BindVertexBuffer(GfxBuffer* vertex_buffer, uint32_t offset) {
        VulkanBuffer* internel_buffer = static_cast<VulkanBuffer*>(vertex_buffer);
        VkBuffer vertexBuffers[] = { internel_buffer->GetHandle() };
        VkDeviceSize offsets[] = { offset };
        vkCmdBindVertexBuffers(_command_buffer, 0, 1, vertexBuffers, offsets);
    }

    void VulkanCommandBuffer::BindIndexBuffer(GfxBuffer* index_buffer, uint32_t offset, IndexType type) {
        VulkanBuffer* internel_buffer = static_cast<VulkanBuffer*>(index_buffer);
        vkCmdBindIndexBuffer(_command_buffer, internel_buffer->GetHandle(), offset, ToVulkanIndexType(type));
    }

    void VulkanCommandBuffer::BindGraphicsPipeline(GfxGraphicsPipeline* pipeline) {
        VulkanGraphicsPipeline* internel_pipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
        vkCmdBindPipeline(_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, internel_pipeline->GetHandle());
    }

    void VulkanCommandBuffer::BindDescriptorSets(GfxRootSignature* root_signature, uint32_t num_sets, GfxDescriptorSet** sets) {
        VulkanRootSignature* internel_root_signature = (VulkanRootSignature*)root_signature;

        std::vector<VkDescriptorSet> internel_sets;
        for (uint32_t i = 0; i < num_sets; ++i) {
            internel_sets.push_back(static_cast<VulkanDescriptorSet*>(sets[i])->GetHandle());
        }

        vkCmdBindDescriptorSets(_command_buffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                internel_root_signature->GetPipelineLayout(),
                                0,
                                internel_sets.size(),
                                internel_sets.data(),
                                0,
                                nullptr);
    }

    void VulkanCommandBuffer::Draw(uint32_t num_vertices, uint32_t num_instances, uint32_t first_vertex, uint32_t first_instance) {
        vkCmdDraw(_command_buffer, num_vertices, num_instances, first_vertex, first_instance);
    }

    void VulkanCommandBuffer::DrawIndexed(uint32_t num_indices, uint32_t num_instances, uint32_t first_index, uint32_t vertex_offset, uint32_t first_instance) {
        vkCmdDrawIndexed(_command_buffer, num_indices, num_instances, first_index, vertex_offset, first_instance);
    }

    void VulkanCommandBuffer::BufferCopyToBuffer(const GfxBufferCopyToBufferRange& range) {
        VulkanBuffer* internel_src_buffer = static_cast<VulkanBuffer*>(range.src_buffer);
        VulkanBuffer* internel_dst_buffer = static_cast<VulkanBuffer*>(range.dst_buffer);
        VkBufferCopy copy = {};
        copy.srcOffset = range.src_offset;
        copy.dstOffset = range.dst_offset;
        copy.size = range.size;
        vkCmdCopyBuffer(_command_buffer, internel_src_buffer->GetHandle(), internel_dst_buffer->GetHandle(), 1, &copy);
    }

    void VulkanCommandBuffer::ImageCopyToImage(const GfxImageCopyToImageRange& range) {
        VulkanTexture* internel_src_texture = static_cast<VulkanTexture*>(range.src_texture);
        VulkanTexture* internel_dst_texture = static_cast<VulkanTexture*>(range.dst_texture);
        VkImageCopy copy = {};
        copy.extent.width = range.width;
        copy.extent.height = range.height;
        copy.extent.depth = range.depth;

        copy.srcSubresource.aspectMask = ToVulkanAspectMask(internel_src_texture->GetFormat());;
        copy.srcSubresource.baseArrayLayer = range.src_layer;
        copy.srcSubresource.mipLevel = range.src_level;
        copy.srcSubresource.layerCount = 1;
        copy.srcOffset = { 0, 0, 0 };

        copy.dstSubresource.aspectMask = ToVulkanAspectMask(internel_dst_texture->GetFormat());;
        copy.dstSubresource.baseArrayLayer = range.dst_layer;
        copy.dstSubresource.mipLevel = range.dst_level;
        copy.dstSubresource.layerCount = 1;
        copy.dstOffset = { 0, 0, 0 };

        vkCmdCopyImage(_command_buffer, internel_src_texture->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, internel_dst_texture->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    }

    void VulkanCommandBuffer::BufferCopyToImage(const GfxBufferCopyToImageRange& range) {
        VulkanBuffer* internel_src_buffer = static_cast<VulkanBuffer*>(range.src_buffer);
        VulkanTexture* internel_dst_texture = static_cast<VulkanTexture*>(range.dst_texture);
        VkBufferImageCopy copy = {};
        copy.bufferOffset = range.buffer_offset;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;
        copy.imageSubresource.aspectMask = ToVulkanAspectMask(internel_dst_texture->GetFormat());
        copy.imageSubresource.mipLevel = range.level;
        copy.imageSubresource.baseArrayLayer = range.layer;
        copy.imageSubresource.layerCount = 1;
        copy.imageOffset.x = 0;
        copy.imageOffset.y = 0;
        copy.imageOffset.z = 0;
        copy.imageExtent.width = internel_dst_texture->GetWidth();
        copy.imageExtent.height = internel_dst_texture->GetHeight();
        copy.imageExtent.depth = internel_dst_texture->GetDepth();

        vkCmdCopyBufferToImage(_command_buffer, internel_src_buffer->GetHandle(), internel_dst_texture->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    }

    void VulkanCommandBuffer::SetBarrier(uint32_t num_buffer_barriers, GfxBufferBarrier* buffer_barriers,
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
            if (!(trans->new_state & buffer->GetResourceState())) {
                flag = true;
                buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                buffer_barrier.pNext = NULL;
                buffer_barrier.srcAccessMask = ToVulkanAccessFlags(buffer->GetResourceState());
                buffer_barrier.dstAccessMask = ToVulkanAccessFlags(trans->new_state);

                buffer->SetResourceState(trans->new_state);
            }
            else if(trans->new_state == RESOURCE_STATE_UNORDERED_ACCESS) {
                flag = true;
                buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                buffer_barrier.pNext = NULL;
                buffer_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
                buffer_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
            }

            if(flag) {
                buffer_barrier.buffer = buffer->GetHandle();
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
            if (!(trans->new_state & texture->GetResourceState())) {
                flag = true;
                image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                image_barrier.pNext = NULL;
                image_barrier.srcAccessMask = ToVulkanAccessFlags(texture->GetResourceState());
                image_barrier.dstAccessMask = ToVulkanAccessFlags(trans->new_state);
                image_barrier.oldLayout = ToVulkanImageLayout(texture->GetResourceState());
                image_barrier.newLayout = ToVulkanImageLayout(trans->new_state);

                texture->SetResourceState(trans->new_state);
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
                image_barrier.image = texture->GetHandle();
                image_barrier.subresourceRange.aspectMask = ToVulkanAspectMask(texture->GetFormat());
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

        VkPipelineStageFlags src_stage_mask = ToPipelineStageFlags(src_access_flags, _pool->GetQueueType());
        VkPipelineStageFlags dst_stage_mask = ToPipelineStageFlags(dst_access_flags, _pool->GetQueueType());

        if (internel_image_barriers.size() || internel_buffer_barriers.size()) {
            vkCmdPipelineBarrier(_command_buffer, src_stage_mask, dst_stage_mask, 0, 0, NULL,
                                 internel_buffer_barriers.size(), internel_buffer_barriers.data(),
                                 internel_image_barriers.size(), internel_image_barriers.data());
        }
    }
}