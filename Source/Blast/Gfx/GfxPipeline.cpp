#include "GfxPipeline.h"

namespace Blast {
    GfxRootSignature::GfxRootSignature(const GfxRootSignatureDesc &desc) {
        mStages = desc.stages;
        if (SHADER_STAGE_VERT == (mStages & SHADER_STAGE_VERT)) {
            mShaderReflections.push_back(desc.vertex);
        }
        if (SHADER_STAGE_TESC == (mStages & SHADER_STAGE_TESC)) {
            mShaderReflections.push_back(desc.hull);
        }
        if (SHADER_STAGE_TESE == (mStages & SHADER_STAGE_TESE)) {
            mShaderReflections.push_back(desc.domain);
        }
        if (SHADER_STAGE_GEOM == (mStages & SHADER_STAGE_GEOM)) {
            mShaderReflections.push_back(desc.geometry);
        }
        if (SHADER_STAGE_FRAG == (mStages & SHADER_STAGE_FRAG)) {
            mShaderReflections.push_back(desc.pixel);
        }
        if (SHADER_STAGE_COMP == (mStages & SHADER_STAGE_COMP)) {
            mShaderReflections.push_back(desc.compute);
        }
    }

    GfxGraphicsPipeline::GfxGraphicsPipeline(const GfxGraphicsPipelineDesc& desc) {
    }
}