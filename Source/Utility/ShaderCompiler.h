#pragma once
#include "Gfx/GfxDefine.h"
#include <vector>
#include <string>

namespace Blast {
    struct ShaderCompileDesc {
        std::string code;
        std::vector<std::string> macros;
        ShaderStage stage;
    };

    class ShaderCompiler {
    public:
        virtual std::vector<uint32_t> compile(const ShaderCompileDesc& desc) = 0;
    };
}