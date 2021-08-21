#include "VulkanPipeline.h"
#include "VulkanContext.h"
#include "VulkanFramebuffer.h"
#include "VulkanShader.h"
#include "VulkanSampler.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"

namespace blast {
    VulkanDescriptorSet::VulkanDescriptorSet(VulkanContext* context, VulkanRootSignature* root_signature, const uint8_t& set) {
        _context = context;
        _root_signature = root_signature;

        VkDescriptorSetAllocateInfo dsai = {};
        dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        dsai.descriptorPool = _context->GetDescriptorPool();
        dsai.descriptorSetCount = 1;
        dsai.pSetLayouts = &root_signature->_set_layout_map[set];
        VK_ASSERT(vkAllocateDescriptorSets(_context->GetDevice(), &dsai, &_set));
    }

    VulkanDescriptorSet::~VulkanDescriptorSet() {
        vkFreeDescriptorSets(_context->GetDevice(), _context->GetDescriptorPool(), 1, &_set);
    }

    void VulkanDescriptorSet::SetSampler(const uint8_t& reg, GfxSampler* sampler) {
        VulkanSampler* internel_sampler = static_cast<VulkanSampler*>(sampler);

        VkDescriptorImageInfo dii{};
        dii.sampler = internel_sampler->GetHandle();

        VkWriteDescriptorSet wds = {};
        wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wds.dstSet = _set;
        wds.dstBinding = reg;
        wds.pImageInfo = &dii;
        wds.descriptorCount = 1;
        wds.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        vkUpdateDescriptorSets(_context->GetDevice(), 1, &wds, 0, nullptr);
    }

    void VulkanDescriptorSet::SetTexture(const uint8_t& reg, GfxTextureView* texture_view) {
        VulkanTextureView* internel_view = static_cast<VulkanTextureView*>(texture_view);

        VkDescriptorImageInfo dii{};
        dii.imageView = internel_view->GetHandle();
        dii.imageLayout = ToVulkanImageLayout(internel_view->GetTexture()->GetResourceState());

        VkWriteDescriptorSet wds = {};
        wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wds.dstSet = _set;
        wds.dstBinding = reg;
        wds.pImageInfo = &dii;
        wds.descriptorCount = 1;
        wds.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        vkUpdateDescriptorSets(_context->GetDevice(), 1, &wds, 0, nullptr);
    }

    void VulkanDescriptorSet::SetStorgeTexture(const uint8_t& reg, GfxTextureView* texture_view) {
        VulkanTextureView* internel_view = static_cast<VulkanTextureView*>(texture_view);

        VkDescriptorImageInfo dii{};
        dii.imageView = internel_view->GetHandle();
        dii.imageLayout = ToVulkanImageLayout(internel_view->GetTexture()->GetResourceState());

        VkWriteDescriptorSet wds = {};
        wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wds.dstSet = _set;
        wds.dstBinding = reg;
        wds.pImageInfo = &dii;
        wds.descriptorCount = 1;
        wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        vkUpdateDescriptorSets(_context->GetDevice(), 1, &wds, 0, nullptr);
    }

    void VulkanDescriptorSet::SetCombinedSampler(const uint8_t& reg, GfxTextureView* texture_view, GfxSampler* sampler) {
        VulkanTextureView* internel_view = static_cast<VulkanTextureView*>(texture_view);
        VulkanSampler* internel_sampler = static_cast<VulkanSampler*>(sampler);

        VkDescriptorImageInfo dii{};
        dii.imageView = internel_view->GetHandle();
        dii.imageLayout = ToVulkanImageLayout(internel_view->GetTexture()->GetResourceState());
        dii.sampler = internel_sampler->GetHandle();

        VkWriteDescriptorSet wds = {};
        wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wds.dstSet = _set;
        wds.dstBinding = reg;
        wds.pImageInfo = &dii;
        wds.descriptorCount = 1;
        wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        vkUpdateDescriptorSets(_context->GetDevice(), 1, &wds, 0, nullptr);
    }

    void VulkanDescriptorSet::SetUniformBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) {
        VulkanBuffer* internel_buffer = static_cast<VulkanBuffer*>(buffer);

        VkDescriptorBufferInfo dbi = {};
        dbi.buffer = internel_buffer->GetHandle();
        dbi.offset = offset;
        dbi.range = size;

        VkWriteDescriptorSet wds = {};
        wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wds.dstSet = _set;
        wds.dstBinding = reg;
        wds.dstArrayElement = 0;
        wds.descriptorCount = 1;
        wds.pBufferInfo = &dbi;
        wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vkUpdateDescriptorSets(_context->GetDevice(), 1, &wds, 0, nullptr);
    }

    void VulkanDescriptorSet::SetStorgeBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) {
        VulkanBuffer* internel_buffer = static_cast<VulkanBuffer*>(buffer);

        VkDescriptorBufferInfo dbi = {};
        dbi.buffer = internel_buffer->GetHandle();
        dbi.offset = offset;
        dbi.range = size;

        VkWriteDescriptorSet wds = {};
        wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wds.dstSet = _set;
        wds.dstBinding = reg;
        wds.dstArrayElement = 0;
        wds.descriptorCount = 1;
        wds.pBufferInfo = &dbi;
        wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        vkUpdateDescriptorSets(_context->GetDevice(), 1, &wds, 0, nullptr);
    }

    VulkanRootSignature::VulkanRootSignature(VulkanContext *context, const GfxRootSignatureDesc &desc)
    :GfxRootSignature(desc) {
        _context = context;

        std::map<int, std::vector<VkDescriptorSetLayoutBinding>> binding_map;
        for (int i = 0; i < desc.registers.size(); ++i) {
            const GfxRegisterInfo& register_info = desc.registers[i];
            VkDescriptorSetLayoutBinding dslb = {};
            dslb.pImmutableSamplers = nullptr;
            dslb.binding = register_info.reg;
            dslb.descriptorCount = register_info.size;
            dslb.descriptorType = ToVulkanDescriptorType(register_info.type);
            dslb.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
            binding_map[register_info.set].push_back(dslb);
        }

        std::vector<VkDescriptorSetLayout> layouts;
        for (auto& binding : binding_map) {
            VkDescriptorSetLayoutCreateInfo dslci = {};
            dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            dslci.bindingCount = binding.second.size();
            dslci.pBindings = binding.second.data();
            VkDescriptorSetLayout layout = VK_NULL_HANDLE;
            VK_ASSERT(vkCreateDescriptorSetLayout(_context->GetDevice(), &dslci, nullptr, &layout));
            _set_layout_map[binding.first] = layout;
            layouts.push_back(layout);
        }

        VkPipelineLayoutCreateInfo plci = {};
        plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        plci.setLayoutCount = layouts.size();
        plci.pSetLayouts = layouts.data();

        VK_ASSERT(vkCreatePipelineLayout(_context->GetDevice(), &plci, nullptr, &_pipeline_layout));
    }

    VulkanRootSignature::~VulkanRootSignature() {
        for (auto& layout : _set_layout_map) {
            vkDestroyDescriptorSetLayout(_context->GetDevice(), layout.second, nullptr);
        }

        vkDestroyPipelineLayout(_context->GetDevice(), _pipeline_layout, nullptr);
    }

    GfxDescriptorSet * VulkanRootSignature::AllocateSet(const uint8_t& set) {
        return new VulkanDescriptorSet(_context, this, set);
    }

    void VulkanRootSignature::DeleteSet(GfxDescriptorSet* set) {
        BLAST_SAFE_DELETE(set);
    }

    static VkPipelineColorBlendStateCreateInfo ToBlendDesc(const GfxBlendState& state, VkPipelineColorBlendAttachmentState* attachments) {
        uint32_t blend_desc_index = 0;
        for (uint32_t i = 0; i < MAX_RENDER_TARGET_ATTACHMENTS; ++i) {
            if (state.target_mask & (1 << i)) {
                VkBool32 blend_enable =
                        (ToVulkanBlendFactor(state.src_factors[blend_desc_index]) != VK_BLEND_FACTOR_ONE ||
                        ToVulkanBlendFactor(state.dst_factors[blend_desc_index]) != VK_BLEND_FACTOR_ZERO ||
                        ToVulkanBlendFactor(state.src_alpha_factors[blend_desc_index]) != VK_BLEND_FACTOR_ONE ||
                        ToVulkanBlendFactor(state.dst_alpha_factors[blend_desc_index]) != VK_BLEND_FACTOR_ZERO);

                attachments[i].blendEnable = blend_enable;
                attachments[i].colorWriteMask = state.color_write_masks[blend_desc_index];
                attachments[i].srcColorBlendFactor = ToVulkanBlendFactor(state.src_factors[blend_desc_index]);
                attachments[i].dstColorBlendFactor = ToVulkanBlendFactor(state.dst_factors[blend_desc_index]);
                attachments[i].colorBlendOp = ToVulkanBlendOp(state.blend_ops[blend_desc_index]);
                attachments[i].srcAlphaBlendFactor = ToVulkanBlendFactor(state.src_alpha_factors[blend_desc_index]);
                attachments[i].dstAlphaBlendFactor = ToVulkanBlendFactor(state.dst_alpha_factors[blend_desc_index]);
                attachments[i].alphaBlendOp = ToVulkanBlendOp(state.blend_alpha_ops[blend_desc_index]);
            }

            if (state.independent_blend) {
                ++blend_desc_index;   
            }
        }

        VkPipelineColorBlendStateCreateInfo pcbsci = {};
        pcbsci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pcbsci.pNext = NULL;
        pcbsci.flags = 0;
        pcbsci.logicOpEnable = VK_FALSE;
        pcbsci.logicOp = VK_LOGIC_OP_CLEAR;
        pcbsci.pAttachments = attachments;
        pcbsci.blendConstants[0] = 0.0f;
        pcbsci.blendConstants[1] = 0.0f;
        pcbsci.blendConstants[2] = 0.0f;
        pcbsci.blendConstants[3] = 0.0f;

        return pcbsci;
    }

    static VkPipelineDepthStencilStateCreateInfo ToDepthDesc(const GfxDepthState& state) {
        VkPipelineDepthStencilStateCreateInfo pdssci = {};
        pdssci.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pdssci.pNext = NULL;
        pdssci.flags = 0;
        pdssci.depthTestEnable = state.depth_test ? VK_TRUE : VK_FALSE;
        pdssci.depthWriteEnable = state.depth_write ? VK_TRUE : VK_FALSE;
        pdssci.depthCompareOp = ToVulkanCompareOp(state.depth_func);
        pdssci.depthBoundsTestEnable = VK_FALSE;
        pdssci.stencilTestEnable = state.stencil_test ? VK_TRUE : VK_FALSE;

        pdssci.front.failOp = ToVulkanStencilOp(state.stencil_front_fail);
        pdssci.front.passOp = ToVulkanStencilOp(state.stencil_front_pass);
        pdssci.front.depthFailOp = ToVulkanStencilOp(state.depth_front_fail);
        pdssci.front.compareOp = ToVulkanCompareOp(state.stencil_front_func);
        pdssci.front.compareMask = state.stencil_read_mask;
        pdssci.front.writeMask = state.stencil_write_mask;
        pdssci.front.reference = 0;

        pdssci.back.failOp = ToVulkanStencilOp(state.stencil_back_fail);
        pdssci.back.passOp = ToVulkanStencilOp(state.stencil_back_pass);
        pdssci.back.depthFailOp = ToVulkanStencilOp(state.depth_back_fail);
        pdssci.back.compareOp = ToVulkanCompareOp(state.stencil_back_func);
        pdssci.back.compareMask = state.stencil_read_mask;
        pdssci.back.writeMask = state.stencil_write_mask;
        pdssci.back.reference = 0;

        pdssci.minDepthBounds = 0;
        pdssci.maxDepthBounds = 1;

        return pdssci;
    }

    static VkPipelineRasterizationStateCreateInfo ToRasterizerDesc(const GfxRasterizerState& state) {
        VkPipelineRasterizationStateCreateInfo prsci = {};
        prsci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        prsci.pNext = NULL;
        prsci.flags = 0;
        prsci.depthClampEnable = state.depth_clamp_enable ? VK_TRUE : VK_FALSE;
        prsci.rasterizerDiscardEnable = VK_FALSE;
        prsci.polygonMode = ToVulkanFillMode(state.fill_mode);
        prsci.cullMode = ToVulkanCullMode(state.cull_mode);
        prsci.frontFace = ToVulkanFrontFace(state.front_face);
        prsci.depthBiasEnable = (state.depth_bias != 0) ? VK_TRUE : VK_FALSE;
        prsci.depthBiasConstantFactor = float(state.depth_bias);
        prsci.depthBiasClamp = 0.0f;
        prsci.depthBiasSlopeFactor = state.slope_scaled_depth_bias;
        prsci.lineWidth = 1;

        return prsci;
    }


    // 1.考虑合并VertexInputState还有InputAssemblyState
    VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanContext *context, const GfxGraphicsPipelineDesc &desc)
        :GfxGraphicsPipeline(desc) {
        _context = context;
        VulkanRootSignature* internel_root_signature = static_cast<VulkanRootSignature*>(desc.root_signature);
        VulkanFramebuffer* internel_framebuffer = static_cast<VulkanFramebuffer*>(desc.framebuffer);

        std::vector<VkPipelineShaderStageCreateInfo> shader_infos;
        if (desc.vertex_shader) {
            VulkanShader* internel_shader = static_cast<VulkanShader*>(desc.vertex_shader);
            VkPipelineShaderStageCreateInfo pssci = {};
            pssci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pssci.stage = VK_SHADER_STAGE_VERTEX_BIT;
            pssci.module = internel_shader->GetHandle();
            pssci.pName = "main";
            shader_infos.push_back(pssci);
        }

        if (desc.hull_shader) {
            VulkanShader* internel_shader = static_cast<VulkanShader*>(desc.hull_shader);
            VkPipelineShaderStageCreateInfo pssci = {};
            pssci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pssci.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            pssci.module = internel_shader->GetHandle();
            pssci.pName = "main";
            shader_infos.push_back(pssci);
        }

        if (desc.domain_shader) {
            VulkanShader* internel_shader = static_cast<VulkanShader*>(desc.domain_shader);
            VkPipelineShaderStageCreateInfo pssci = {};
            pssci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pssci.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            pssci.module = internel_shader->GetHandle();
            pssci.pName = "main";
            shader_infos.push_back(pssci);
        }

        if (desc.geometry_shader) {
            VulkanShader* internel_shader = static_cast<VulkanShader*>(desc.geometry_shader);
            VkPipelineShaderStageCreateInfo pssci = {};
            pssci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pssci.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            pssci.module = internel_shader->GetHandle();
            pssci.pName = "main";
            shader_infos.push_back(pssci);
        }

        if (desc.pixel_shader) {
            VulkanShader* internel_shader = static_cast<VulkanShader*>(desc.pixel_shader);
            VkPipelineShaderStageCreateInfo pssci = {};
            pssci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            pssci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            pssci.module = internel_shader->GetHandle();
            pssci.pName = "main";
            shader_infos.push_back(pssci);
        }

        // 设置当前的管线的顶点布局
        uint32_t num_input_bindings = 0;
        VkVertexInputBindingDescription input_bindings[MAX_VERTEX_BINDINGS] = { { 0 } };
        uint32_t num_input_attributes = 0;
        VkVertexInputAttributeDescription input_attributes[MAX_VERTEX_ATTRIBS] = { { 0 } };
        uint32_t num_attributes = desc.vertex_layout.num_attributes > MAX_VERTEX_ATTRIBS ? MAX_VERTEX_ATTRIBS : desc.vertex_layout.num_attributes;
        uint32_t binding_value = UINT32_MAX;
        for (uint32_t i = 0; i < num_attributes; ++i) {
            const GfxVertexAttribute* attribute = &(desc.vertex_layout.attributes[i]);

            if (binding_value != attribute->binding) {
                binding_value = attribute->binding;
                ++num_input_bindings;
            }

            input_bindings[num_input_bindings - 1].binding = binding_value;
            if (attribute->rate == VERTEX_ATTRIB_RATE_INSTANCE) {
                input_bindings[num_input_bindings - 1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            } else {
                input_bindings[num_input_bindings - 1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }
            input_bindings[num_input_bindings - 1].stride += GetFormatStride(attribute->format);

            input_attributes[num_input_attributes].location = attribute->location;
            input_attributes[num_input_attributes].binding = attribute->binding;
            input_attributes[num_input_attributes].format = ToVulkanFormat(attribute->format);
            input_attributes[num_input_attributes].offset = attribute->offset;
            ++num_input_attributes;
        }
        VkPipelineVertexInputStateCreateInfo pvisci = {};
        pvisci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pvisci.pNext = NULL;
        pvisci.flags = 0;
        pvisci.vertexBindingDescriptionCount = num_input_bindings;
        pvisci.pVertexBindingDescriptions = input_bindings;
        pvisci.vertexAttributeDescriptionCount = num_input_attributes;
        pvisci.pVertexAttributeDescriptions = input_attributes;

        // 设置当前管线的图元单位
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        switch (desc.rasterizer_state.primitive_topo) {
            case PRIMITIVE_TOPO_POINT_LIST: topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
            case PRIMITIVE_TOPO_LINE_LIST: topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
            case PRIMITIVE_TOPO_LINE_STRIP: topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
            case PRIMITIVE_TOPO_TRI_STRIP: topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
            case PRIMITIVE_TOPO_PATCH_LIST: topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; break;
            case PRIMITIVE_TOPO_TRI_LIST: topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        }
        VkPipelineInputAssemblyStateCreateInfo piasci = {};
        piasci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        piasci.pNext = NULL;
        piasci.flags = 0;
        piasci.topology = topology;
        piasci.primitiveRestartEnable = VK_FALSE;

        // 设置当前管线的viewport,目前由外部动态设置
        VkPipelineViewportStateCreateInfo pvsci = {};
        pvsci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pvsci.pNext = NULL;
        pvsci.flags = 0;
        pvsci.viewportCount = 1;
        pvsci.pViewports = NULL;
        pvsci.scissorCount = 1;
        pvsci.pScissors = NULL;

        // 设置当前管线的多重采样
        VkPipelineMultisampleStateCreateInfo pmsci = {};
        pmsci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pmsci.pNext = NULL;
        pmsci.flags = 0;
        pmsci.rasterizationSamples = ToVulkanSampleCount(internel_framebuffer->GetSampleCount());
        pmsci.sampleShadingEnable = VK_FALSE;
        pmsci.minSampleShading = 0.0f;
        pmsci.pSampleMask = 0;
        pmsci.alphaToCoverageEnable = VK_FALSE;
        pmsci.alphaToOneEnable = VK_FALSE;

        VkPipelineRasterizationStateCreateInfo prsci = {};
        prsci = ToRasterizerDesc(desc.rasterizer_state);

        VkPipelineDepthStencilStateCreateInfo pdssci = {};
        pdssci = ToDepthDesc(desc.depth_state);

        // 设置当前管线的混合状态
        VkPipelineColorBlendAttachmentState blend_attachments[MAX_RENDER_TARGET_ATTACHMENTS];
        VkPipelineColorBlendStateCreateInfo pcbsci = {};
        pcbsci = ToBlendDesc(desc.blend_state, blend_attachments);
        pcbsci.attachmentCount = internel_framebuffer->GetColorAttachmentCount();

        VkDynamicState dynamic_states[6];
        dynamic_states[0] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamic_states[1] = VK_DYNAMIC_STATE_SCISSOR;
        dynamic_states[2] = VK_DYNAMIC_STATE_DEPTH_BIAS;
        dynamic_states[3] = VK_DYNAMIC_STATE_BLEND_CONSTANTS;
        dynamic_states[4] = VK_DYNAMIC_STATE_DEPTH_BOUNDS;
        dynamic_states[5] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;

        VkPipelineDynamicStateCreateInfo pdsci = {};
        pdsci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pdsci.pNext = NULL;
        pdsci.flags = 0;
        pdsci.dynamicStateCount = 6;
        pdsci.pDynamicStates = dynamic_states;

        VkGraphicsPipelineCreateInfo gpci = {};
        gpci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        gpci.pNext = NULL;
        gpci.flags = 0;
        gpci.stageCount = shader_infos.size();
        gpci.pStages = shader_infos.data();
        gpci.pVertexInputState = &pvisci;
        gpci.pInputAssemblyState = &piasci;
        gpci.pTessellationState = NULL;
        gpci.pViewportState = &pvsci;
        gpci.pRasterizationState = &prsci;
        gpci.pMultisampleState = &pmsci;
        gpci.pDepthStencilState = &pdssci;
        gpci.pColorBlendState = &pcbsci;
        gpci.pDynamicState = &pdsci;
        gpci.layout = internel_root_signature->GetPipelineLayout();
        gpci.renderPass = internel_framebuffer->GetRenderPass();
        gpci.subpass = 0;
        gpci.basePipelineHandle = VK_NULL_HANDLE;
        gpci.basePipelineIndex = -1;
        VK_ASSERT(vkCreateGraphicsPipelines(_context->GetDevice(), VK_NULL_HANDLE, 1, &gpci, nullptr, &_pipeline));

    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
        vkDestroyPipeline(_context->GetDevice(), _pipeline, nullptr);
    }
}