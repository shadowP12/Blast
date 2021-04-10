#pragma once
#include "Blast/Gfx/GfxDefine.h"
#include "Blast/Gfx/GfxShader.h"
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