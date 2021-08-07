#include "GfxPipeline.h"

namespace blast {
    GfxRootSignature::GfxRootSignature(const GfxRootSignatureDesc& desc) {
        _registers = desc.registers;
    }

    GfxGraphicsPipeline::GfxGraphicsPipeline(const GfxGraphicsPipelineDesc& desc) {
    }
}