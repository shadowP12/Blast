#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "VulkanRenderTarget.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanPipeline.h"

namespace Blast {
    VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanContext* context, const GfxCommandBufferPoolDesc& desc)
        :GfxCommandBufferPool(desc) {
        mContext = context;
        VulkanQueue* queue = static_cast<VulkanQueue*>(desc.queue);
        mQueueType = queue->getType();
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queue->getFamilyIndex();
        if (!desc.transient)
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        else
            poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        VK_ASSERT(vkCreateCommandPool(mContext->getDevice(), &poolInfo, nullptr, &mPool));
    }

    VulkanCommandBufferPool::~VulkanCommandBufferPool() {
        vkDestroyCommandPool(mContext->getDevice(), mPool, nullptr);
    }

    GfxCommandBuffer * VulkanCommandBufferPool::allocBuf(bool secondary) {
        GfxCommandBufferDesc desc;
        desc.pool = this;
        desc.secondary = secondary;
        VulkanCommandBuffer* cmd = new VulkanCommandBuffer(mContext, desc);
        return cmd;
    }

    VulkanCommandBuffer::VulkanCommandBuffer(VulkanContext* context, const GfxCommandBufferDesc& desc)
        :GfxCommandBuffer(desc) {
        mContext = context;
        mPool = static_cast<VulkanCommandBufferPool*>(desc.pool);

        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = mPool->getHandle();
        allocateInfo.commandBufferCount = 1;
        if (!desc.secondary)
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        else
            allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;

        VK_ASSERT(vkAllocateCommandBuffers(mContext->getDevice(), &allocateInfo, &mCommandBuffer));
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() {
        vkFreeCommandBuffers(mContext->getDevice(), mPool->getHandle(), 1, &mCommandBuffer);
    }

    void VulkanCommandBuffer::begin() {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_ASSERT(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
    }

    void VulkanCommandBuffer::end() {
        VK_ASSERT(vkEndCommandBuffer(mCommandBuffer));
    }

    void VulkanCommandBuffer::bindRenderTarget(GfxRenderPass* renderPass, GfxFramebuffer* framebuffer, const GfxClearValue& clearValue) {
        VulkanRenderPass* internelRenderPass = (VulkanRenderPass*)renderPass;
        VulkanFramebuffer* internelFramebuffer = (VulkanFramebuffer*)framebuffer;
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = internelRenderPass->getHandle();
        renderPassInfo.framebuffer = internelFramebuffer->getHandle();
        renderPassInfo.renderArea.offset.x = 0;
        renderPassInfo.renderArea.offset.y = 0;
        renderPassInfo.renderArea.extent.width = internelFramebuffer->getWidth();
        renderPassInfo.renderArea.extent.height = internelFramebuffer->getHeight();

        VkClearValue clearValues[2];
        clearValues[0].color = { clearValue.color[0], clearValue.color[1], clearValue.color[2], clearValue.color[3] };
        clearValues[1].depthStencil = { clearValue.depth, clearValue.stencil };
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandBuffer::unbindRenderTarget() {
        vkCmdEndRenderPass(mCommandBuffer);
    }

    void VulkanCommandBuffer::setViewport(float x, float y, float w, float h) {
        VkViewport viewport = {};
        viewport.x = x;
        viewport.y = y;
        viewport.width = w;
        viewport.height = h;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
    }

    void VulkanCommandBuffer::setScissor(int x, int y, int w, int h) {
        VkRect2D scissor = {};
        scissor.offset.x = x;
        scissor.offset.y = y;
        scissor.extent.width = w;
        scissor.extent.height = h;
        vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
    }

    void VulkanCommandBuffer::bindVertexBuffer(GfxBuffer* vertexBuffer, uint32_t offset) {
        VulkanBuffer* internelBuffer = static_cast<VulkanBuffer*>(vertexBuffer);
        VkBuffer vertexBuffers[] = { internelBuffer->getHandle() };
        VkDeviceSize offsets[] = { offset };
        vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, vertexBuffers, offsets);
    }

    void VulkanCommandBuffer::bindIndexBuffer(GfxBuffer* indexBuffer, uint32_t offset, IndexType type) {
        VulkanBuffer* internelBuffer = static_cast<VulkanBuffer*>(indexBuffer);
        vkCmdBindIndexBuffer(mCommandBuffer, internelBuffer->getHandle(), offset, toVulkanIndexType(type));
    }

    void VulkanCommandBuffer::bindGraphicsPipeline(GfxGraphicsPipeline* pipeline) {
        VulkanGraphicsPipeline* internelPipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
        vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, internelPipeline->getHandle());
    }

    void VulkanCommandBuffer::bindRootSignature(GfxRootSignature* rootSignature) {
        VulkanRootSignature* internelRootSignature = static_cast<VulkanRootSignature*>(rootSignature);
        ShaderStage stages = internelRootSignature->getShaderStages();

        if (SHADER_STAGE_VERT == (stages & SHADER_STAGE_VERT)) {
            vkCmdBindDescriptorSets(mCommandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    internelRootSignature->getPipelineLayout(),
                                    0,
                                    internelRootSignature->getSets().size(),
                                    internelRootSignature->getSets().data(),
                                    0,
                                    nullptr);
        }

        if (SHADER_STAGE_COMP == (stages & SHADER_STAGE_COMP)) {
            vkCmdBindDescriptorSets(mCommandBuffer,
                                    VK_PIPELINE_BIND_POINT_COMPUTE,
                                    internelRootSignature->getPipelineLayout(),
                                    0,
                                    internelRootSignature->getSets().size(),
                                    internelRootSignature->getSets().data(),
                                    0,
                                    nullptr);
        }
    }

    void VulkanCommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
        vkCmdDraw(mCommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VulkanCommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
                                           uint32_t vertexOffset, uint32_t firstInstance) {
        vkCmdDrawIndexed(mCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void VulkanCommandBuffer::copyToBuffer(GfxBuffer* srcBuffer, uint32_t srcOffset, GfxBuffer* dstBuffer,
                                           uint32_t dstOffset, uint32_t size) {
        VulkanBuffer* internelSrcBuffer = static_cast<VulkanBuffer*>(srcBuffer);
        VulkanBuffer* internelDstBuffer = static_cast<VulkanBuffer*>(dstBuffer);
        VkBufferCopy copy = {};
        copy.srcOffset = srcOffset;
        copy.dstOffset = dstOffset;
        copy.size = size;
        vkCmdCopyBuffer(mCommandBuffer, internelSrcBuffer->getHandle(), internelDstBuffer->getHandle(), 1, &copy);
    }

    void VulkanCommandBuffer::copyToImage(GfxBuffer* srcBuffer, GfxTexture* dstTexture,
                                          const GfxCopyToImageHelper& helper) {
        VulkanBuffer* internelSrcBuffer = static_cast<VulkanBuffer*>(srcBuffer);
        VulkanTexture* internelDstTexture = static_cast<VulkanTexture*>(dstTexture);
        VkBufferImageCopy copy = {};
        copy.bufferOffset = helper.bufferOffset;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;
        copy.imageSubresource.aspectMask = toVulkanAspectMask(dstTexture->getFormat());
        copy.imageSubresource.mipLevel = helper.level;
        copy.imageSubresource.baseArrayLayer = helper.layer;
        copy.imageSubresource.layerCount = 1;
        copy.imageOffset.x = 0;
        copy.imageOffset.y = 0;
        copy.imageOffset.z = 0;
        copy.imageExtent.width = dstTexture->getWidth();
        copy.imageExtent.height = dstTexture->getHeight();
        copy.imageExtent.depth = dstTexture->getDepth();

        vkCmdCopyBufferToImage(mCommandBuffer, internelSrcBuffer->getHandle(), internelDstTexture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    }

    void VulkanCommandBuffer::setBarrier(uint32_t inBufferBarrierCount, GfxBufferBarrier* inBufferBarriers,
                                         uint32_t inTextureBarrierCount, GfxTextureBarrier* inTextureBarriers) {
        std::vector<VkImageMemoryBarrier> imageBarriers;
        std::vector<VkBufferMemoryBarrier> bufferBarriers;

        VkAccessFlags srcAccessFlags = 0;
        VkAccessFlags dstAccessFlags = 0;

        for (uint32_t i = 0; i < inBufferBarrierCount; ++i) {
            GfxBufferBarrier* trans = &inBufferBarriers[i];
            VulkanBuffer* buffer = static_cast<VulkanBuffer*>(trans->buffer);
            bool flag = false;
            VkBufferMemoryBarrier bufferBarrier = {};
            if (!(trans->newState & buffer->getResourceState())) {
                flag = true;
                bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                bufferBarrier.pNext = NULL;
                bufferBarrier.srcAccessMask = toVulkanAccessFlags(buffer->getResourceState());
                bufferBarrier.dstAccessMask = toVulkanAccessFlags(trans->newState);

                buffer->setResourceState(trans->newState);
            }
            else if(trans->newState == RESOURCE_STATE_UNORDERED_ACCESS) {
                flag = true;
                bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                bufferBarrier.pNext = NULL;
                bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
                bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
            }

            if(flag) {
                bufferBarrier.buffer = buffer->getHandle();
                bufferBarrier.size = VK_WHOLE_SIZE;
                bufferBarrier.offset = 0;
                bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                srcAccessFlags |= bufferBarrier.srcAccessMask;
                dstAccessFlags |= bufferBarrier.dstAccessMask;
                bufferBarriers.push_back(bufferBarrier);
            }
        }

        for (uint32_t i = 0; i < inTextureBarrierCount; ++i) {
            GfxTextureBarrier* trans = &inTextureBarriers[i];
            VulkanTexture* texture = static_cast<VulkanTexture*>(trans->texture);
            bool flag = false;
            VkImageMemoryBarrier imageBarrier = {};
            if (!(trans->newState & texture->getResourceState())) {
                flag = true;
                imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imageBarrier.pNext = NULL;
                imageBarrier.srcAccessMask = toVulkanAccessFlags(texture->getResourceState());
                imageBarrier.dstAccessMask = toVulkanAccessFlags(trans->newState);
                imageBarrier.oldLayout = toVulkanImageLayout(texture->getResourceState());
                imageBarrier.newLayout = toVulkanImageLayout(trans->newState);

                texture->setResourceState(trans->newState);
            }
            else if(trans->newState == RESOURCE_STATE_UNORDERED_ACCESS) {
                flag = true;
                imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imageBarrier.pNext = NULL;
                imageBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            }

            if(flag) {
                imageBarrier.image = texture->getImage();
                imageBarrier.subresourceRange.aspectMask = toVulkanAspectMask(texture->getFormat());
                imageBarrier.subresourceRange.baseMipLevel = 0;
                imageBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
                imageBarrier.subresourceRange.baseArrayLayer = 0;
                imageBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
                imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                srcAccessFlags |= imageBarrier.srcAccessMask;
                dstAccessFlags |= imageBarrier.dstAccessMask;
                imageBarriers.push_back(imageBarrier);
            }
        }

        VkPipelineStageFlags srcStageMask = toPipelineStageFlags(srcAccessFlags, mPool->getQueueType());
        VkPipelineStageFlags dstStageMask = toPipelineStageFlags(dstAccessFlags, mPool->getQueueType());

        if (imageBarriers.size() || bufferBarriers.size()) {
            vkCmdPipelineBarrier(mCommandBuffer, srcStageMask, dstStageMask, 0, 0, NULL,
                                 bufferBarriers.size(), bufferBarriers.data(),
                                 imageBarriers.size(), imageBarriers.data());
        }
    }
}