#pragma once
#include "Blast/Gfx/GfxDefine.h"
#include "Blast/Gfx/GfxShader.h"
#include <vector>
#include <string>

namespace blast {

    struct GfxShaderResource {
        std::string name;
        uint32_t set;
        uint32_t reg;
        uint32_t size;
        ResourceType type;
        TextureDimension dim;
    };

    struct GfxShaderVariable {
        std::string name;
        uint16_t parent_index;
        uint16_t offset;
        uint16_t size;
        uint16_t count;
        UniformType type;
    };

    struct ShaderCompileDesc {
        std::string code;
        std::string preamble;
        std::vector<std::string> include_dirs;
        ShaderStage stage;
    };

    struct ShaderCompileResult {
        bool success;
        std::vector<uint32_t> bytes;
        std::vector<GfxShaderResource> resources;
        std::vector<GfxShaderVariable> variables;
    };

    class ShaderCompiler {
    public:
        virtual ShaderCompileResult Compile(const ShaderCompileDesc& desc) = 0;
    };
}