#pragma once
#include "GfxDefine.h"
#include "GfxShader.h"
#include <string>
#include <vector>

namespace Blast {
    class GfxBuffer;
    class GfxTexture;
    class GfxSampler;
    class GfxRenderPass;

    class GfxDescriptorSet {
    public:
        GfxDescriptorSet() = default;
        virtual ~GfxDescriptorSet() = default;
        virtual void setSampler(const uint8_t& reg, GfxSampler* sampler) = 0;
        virtual void setTexture(const uint8_t& reg, GfxTexture* texture) = 0;
        virtual void setCombinedSampler(const uint8_t& reg, GfxTexture* texture, GfxSampler* sampler) = 0;
        virtual void setRWTexture(const uint8_t& reg, GfxTexture* texture) = 0;
        virtual void setUniformBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) = 0;
        virtual void setRWBuffer(const uint8_t& reg, GfxBuffer* buffer, uint32_t size, uint32_t offset) = 0;
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
        virtual GfxDescriptorSet* allocateSet(const uint8_t& set) = 0;
    protected:
        std::vector<GfxRegisterInfo> mRegisters;
    };

    struct GfxVertexAttrib {
        uint32_t binding = 0;
        uint32_t location = 0;
        uint32_t offset = 0;
        Format format = FORMAT_UNKNOWN;
        ShaderSemantic semantic = SEMANTIC_UNDEFINED;
        VertexAttribRate rate = VERTEX_ATTRIB_RATE_VERTEX;
    };

    struct GfxVertexLayout {
        uint32_t attribCount = 0;
        GfxVertexAttrib attribs[MAX_VERTEX_ATTRIBS];
    };

    struct GfxBlendState {
        BlendConstant srcFactors[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendConstant dstFactors[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendConstant srcAlphaFactors[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendConstant dstAlphaFactors[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendOp blendOps[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendOp blendAlphaOps[MAX_RENDER_TARGET_ATTACHMENTS] = {};
        BlendStateTargets targetMask = BLEND_STATE_TARGET_ALL; // 默认当前Pass里面的所有RT都受Blend影响
        uint32_t masks[MAX_RENDER_TARGET_ATTACHMENTS] = {}; // 该mask用来设置最终写入RT的Color Component
        bool independentBlend = false; // 默认所有RT统一使用相同设置
    };

    struct GfxDepthState {
        bool depthTest = false;
        bool depthWrite = true;
        CompareMode depthFunc = COMPARE_LEQUAL;
        bool stencilTest = false;
        uint8_t stencilReadMask = 0xFF; // 默认都可读
        uint8_t stencilWriteMask = 0xFF; // 默认都可写
        CompareMode stencilFrontFunc = COMPARE_ALWAYS;
        StencilOp stencilFrontFail = STENCIL_OP_KEEP;
        StencilOp depthFrontFail = STENCIL_OP_KEEP;
        StencilOp stencilFrontPass = STENCIL_OP_KEEP;
        CompareMode stencilBackFunc = COMPARE_ALWAYS;
        StencilOp stencilBackFail = STENCIL_OP_KEEP;
        StencilOp depthBackFail = STENCIL_OP_KEEP;
        StencilOp stencilBackPass = STENCIL_OP_KEEP;
    };

    struct GfxRasterizerState {
        int32_t depthBias = 0; // 默认为0
        float slopeScaledDepthBias = 0.0; // 默认为0
        bool multiSample = 1; // 默认一像素采样
        bool depthClampEnable = false;
        PrimitiveTopology primitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
        FillMode fillMode = FILL_MODE_SOLID;
        FrontFace frontFace = FRONT_FACE_CW;
        CullMode cullMode = CULL_MODE_NONE;
    };

    struct GfxGraphicsPipelineDesc {
        GfxRenderPass* renderPass = nullptr;
        GfxShader* vertexShader = nullptr;
        GfxShader* hullShader = nullptr;
        GfxShader* domainShader = nullptr;
        GfxShader* geometryShader = nullptr;
        GfxShader* pixelShader = nullptr;
        GfxRootSignature* rootSignature = nullptr;
        GfxVertexLayout vertexLayout;
        GfxBlendState blendState;
        GfxDepthState depthState;
        GfxRasterizerState rasterizerState;
    };

    class GfxGraphicsPipeline {
    public:
        GfxGraphicsPipeline(const GfxGraphicsPipelineDesc& desc);
        virtual ~GfxGraphicsPipeline() = default;
    protected:
    };
}