#pragma once
#include "GfxDefine.h"
#include <string>
#include <vector>

namespace blast {
    struct GfxShaderDesc {
        std::vector<uint32_t> bytes;
        ShaderStage stage;
    };

    class GfxShader {
    public:
        GfxShader(const GfxShaderDesc& desc);

        virtual ~GfxShader() = default;

    protected:
        ShaderStage _stage;
    };
}