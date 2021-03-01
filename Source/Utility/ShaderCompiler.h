#pragma once
#include "Gfx/GfxDefine.h"
#include "Gfx/GfxShader.h"
#include <vector>
#include <string>

namespace Blast {
    struct ShaderCompileDesc {
        std::string code;
        std::string preamble;
        std::vector<std::string> includeDirs;
        ShaderStage stage;
    };

    struct ShaderCompileResult {
        std::vector<uint32_t> bytes;
        GfxShaderReflection reflection;
    };

    class ShaderCompiler {
    public:
        virtual ShaderCompileResult compile(const ShaderCompileDesc& desc) = 0;
    };
}