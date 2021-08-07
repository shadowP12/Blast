#pragma once
#include "GfxDefine.h"
#include "GfxShader.h"
#include <string>
#include <vector>

namespace blast {
    class GfxBuffer;
    class GfxTexture;
    class GfxTextureView;
    class GfxSampler;
    class GfxFramebuffer;

    class GfxDescriptorSet {
    public:
        GfxDescriptorSet() = default;

        virtual ~GfxDescriptorSet() = default;

        virtual void SetSampler(const uint8_t& reg, GfxSampler* sampler) = 0;

        virtual void SetTexture(const uint8_t& reg, GfxTextureView* texture_view) = 0;

        virtual void SetStorgeTexture(const uint8_t& reg, GfxTextureView* texture_view) = 0;

        virtual void SetCombinedSampler(const uint8_t& reg, GfxTextureView* texture_view, GfxSampler* sampler) = 0;

        virtual void SetUniformBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) = 0;

        virtual void SetStorgeBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) = 0;
    };

    // Shader里面"寄存器"的信息
    struct GfxRegisterInfo {
        uint16_t set;
        uint16_t reg;
        uint16_t size;
        ResourceType type;
    };

    struct GfxRootSignatureDesc {
        std::vector<GfxRegisterInfo> registers;
    };

    // Shader的参数布局模板
    class GfxRootSignature {
    public:
        GfxRootSignature(const GfxRootSignatureDesc& desc);

        virtual ~GfxRootSignature() = default;

        virtual GfxDescriptorSet* AllocateSet(const uint8_t& set) = 0;

        virtual void DeleteSet(GfxDescriptorSet* set) = 0;

    protected:
        std::vector<GfxRegisterInfo> _registers;
    };

    struct GfxVertexAttribute {
        uint32_t binding = 0;
        uint32_t location = 0;
        uint32_t offset = 0;
        uint32_t size = 0;
        Format format = FORMAT_UNKNOWN;
        ShaderSemantic semantic = SEMANTIC_UNDEFINED;
        VertexAttribRate rate = VERTEX_ATTRIB_RATE_VERTEX;
    };

    struct GfxVertexLayout {
        uint32_t num_attributes = 0;
        GfxVertexAttribute attributes[MAX_VERTEX_ATTRIBS];
    };

    struct GfxBlendState {
        BlendConstant src_factors[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendConstant dst_factors[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendConstant src_alpha_factors[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendConstant dst_alpha_factors[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendOp blend_ops[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendOp blend_alpha_ops[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        // 该mask用来设置最终写入RT的Color Component
        ColorComponentFlag color_write_masks[MAX_RENDER_TARGET_ATTACHMENTS] = { COLOR_COMPONENT_ALL };
        // 默认当前Pass里面的所有RT都受Blend影响
        BlendStateTargets target_mask = BLEND_STATE_TARGET_ALL;
        // 默认所有RT统一使用相同设置
        bool independent_blend = false;
    };

    struct GfxDepthState {
        bool depth_test = false;
        bool depth_write = true;
        CompareMode depth_func = COMPARE_LEQUAL;
        bool stencil_test = false;
        // 默认都可读
        uint8_t stencil_read_mask = 0xFF;
        // 默认都可写
        uint8_t stencil_write_mask = 0xFF;
        CompareMode stencil_front_func = COMPARE_ALWAYS;
        StencilOp stencil_front_fail = STENCIL_OP_KEEP;
        StencilOp depth_front_fail = STENCIL_OP_KEEP;
        StencilOp stencil_front_pass = STENCIL_OP_KEEP;
        CompareMode stencil_back_func = COMPARE_ALWAYS;
        StencilOp stencil_back_fail = STENCIL_OP_KEEP;
        StencilOp depth_back_fail = STENCIL_OP_KEEP;
        StencilOp stencil_back_pass = STENCIL_OP_KEEP;
    };

    struct GfxRasterizerState {
        // 默认为0
        int32_t depth_bias = 0;
        // 默认为0
        float slope_scaled_depth_bias = 0.0;
        bool depth_clamp_enable = false;
        PrimitiveTopology primitive_topo = PRIMITIVE_TOPO_TRI_LIST;
        FillMode fill_mode = FILL_MODE_SOLID;
        FrontFace front_face = FRONT_FACE_CW;
        CullMode cull_mode = CULL_MODE_NONE;
    };

    struct GfxGraphicsPipelineDesc {
        GfxFramebuffer* framebuffer = nullptr;
        GfxShader* vertex_shader = nullptr;
        GfxShader* hull_shader = nullptr;
        GfxShader* domain_shader = nullptr;
        GfxShader* geometry_shader = nullptr;
        GfxShader* pixel_shader = nullptr;
        GfxRootSignature* root_signature = nullptr;
        GfxVertexLayout vertex_layout;
        GfxBlendState blend_state;
        GfxDepthState depth_state;
        GfxRasterizerState rasterizer_state;
    };

    class GfxGraphicsPipeline {
    public:
        GfxGraphicsPipeline(const GfxGraphicsPipelineDesc& desc);

        virtual ~GfxGraphicsPipeline() = default;

    protected:
    };
}