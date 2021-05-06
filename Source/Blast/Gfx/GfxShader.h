#pragma once
#include "GfxDefine.h"
#include <string>
#include <vector>

namespace Blast {
    struct GfxShaderDesc {
        std::vector<uint32_t> bytes;
        ShaderStage stage;
    };

    class GfxShader {
    public:
        GfxShader(const GfxShaderDesc& desc);
    protected:
        ShaderStage mStage;
    };
}