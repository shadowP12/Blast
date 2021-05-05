#include "GfxPipeline.h"

namespace Blast {
    GfxRootSignature::GfxRootSignature(const GfxRootSignatureDesc& desc) {
        mRegisters = desc.registers;
    }

    GfxGraphicsPipeline::GfxGraphicsPipeline(const GfxGraphicsPipelineDesc& desc) {
    }
}