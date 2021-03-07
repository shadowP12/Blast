#pragma once
#include "GfxDefine.h"
#include "GfxShader.h"
#include <string>
#include <vector>

namespace Blast {
    class GfxRenderPass;

    struct GfxRootSignatureDesc {
        GfxShaderReflection vertex;
        GfxShaderReflection hull;
        GfxShaderReflection domain;
        GfxShaderReflection geometry;
        GfxShaderReflection pixel;
        GfxShaderReflection compute;
        ShaderStage stages;
    };

    class GfxRootSignature {
    public:
        GfxRootSignature(const GfxRootSignatureDesc& desc);
        ShaderStage getShaderStages() { return mStages; }
    protected:
        ShaderStage mStages;
        std::vector<GfxShaderReflection> mShaderReflections;
    };

    struct GfxVertexAttrib {
        std::string name;
        uint32_t binding;
        uint32_t location;
        uint32_t offset;
        Format format;
        ShaderSemantic semantic;
        VertexAttribRate rate;
    };

    struct GfxVertexLayout {
        uint32_t attribCount;
        GfxVertexAttrib attribs[MAX_VERTEX_ATTRIBS];
    };

    struct GfxBlendState {
        BlendConstant srcFactors[MAX_RENDER_TARGET_ATTACHMENTS];
        BlendConstant dstFactors[MAX_RENDER_TARGET_ATTACHMENTS];
        BlendConstant srcAlphaFactors[MAX_RENDER_TARGET_ATTACHMENTS];
        BlendConstant dstAlphaFactors[MAX_RENDER_TARGET_ATTACHMENTS];
        BlendOp blendOps[MAX_RENDER_TARGET_ATTACHMENTS];
        BlendOp blendAlphaOps[MAX_RENDER_TARGET_ATTACHMENTS];
        BlendStateTargets targetMask = BLEND_STATE_TARGET_ALL; // 默认当前Pass里面的所有RT都受Blend影响
        uint32_t masks[MAX_RENDER_TARGET_ATTACHMENTS]; // 该mask用来设置最终写入RT的Color Component
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
        FillMode fillMode;
        FrontFace frontFace;
        CullMode cullMode;
    };

    struct GfxGraphicsPipelineDesc {
        GfxRenderPass* renderPass = nullptr;
        GfxShader* vertexShader = nullptr;
        GfxShader* hullShader = nullptr;
        GfxShader* domainShader = nullptr;
        GfxShader* geometryShader = nullptr;
        GfxShader* pixelShader = nullptr;
        GfxRootSignature* rootSignature = nullptr;
        GfxVertexLayout* vertexLayout = nullptr;
        GfxBlendState* blendState = nullptr;
        GfxDepthState* depthState = nullptr;
        GfxRasterizerState* rasterizerState = nullptr;
        PrimitiveTopology primitiveTopo;
    };

    class GfxGraphicsPipeline {
    public:
        GfxGraphicsPipeline(const GfxGraphicsPipelineDesc& desc);
    protected:
    };
}