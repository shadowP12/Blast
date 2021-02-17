#pragma once
#include "GfxDefine.h"
#include "GfxShader.h"
#include <string>
#include <vector>

namespace Blast {
    class GfxRenderTarget;

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
        BlendStateTargets targetMask;
        uint32_t masks[MAX_RENDER_TARGET_ATTACHMENTS];
        bool independentBlend;
    };

    struct GfxDepthState {
        bool depthTest;
        bool depthWrite;
        CompareMode depthFunc;
        bool stencilTest;
        uint8_t stencilReadMask;
        uint8_t stencilWriteMask;
        CompareMode stencilFrontFunc;
        StencilOp stencilFrontFail;
        StencilOp depthFrontFail;
        StencilOp stencilFrontPass;
        CompareMode stencilBackFunc;
        StencilOp stencilBackFail;
        StencilOp depthBackFail;
        StencilOp stencilBackPass;
    };

    struct GfxRasterizerState {
        int32_t depthBias;
        float slopeScaledDepthBias;
        bool multiSample;
        bool scissor;
        bool depthClampEnable;
        FillMode fillMode;
        FrontFace frontFace;
        CullMode cullMode;
    };

    struct GfxGraphicsPipelineDesc {
        GfxRenderTarget* renderTarget = nullptr;
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