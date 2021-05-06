#pragma once
#include "Blast/Gfx/GfxDefine.h"
#include "Blast/Gfx/GfxShader.h"
#include <vector>
#include <string>

namespace Blast {
    enum class UniformType {
        UNDEFINED,
        BOOL,
        BOOL2,
        BOOL3,
        BOOL4,
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        INT,
        INT2,
        INT3,
        INT4,
        UINT,
        UINT2,
        UINT3,
        UINT4,
        MAT3,
        MAT4
    };

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
        uint32_t parentIndex;
        uint32_t offset;
        uint32_t size;
        UniformType type;
    };

    struct ShaderCompileDesc {
        std::string code;
        std::string preamble;
        std::vector<std::string> includeDirs;
        ShaderStage stage;
    };

    struct ShaderCompileResult {
        std::vector<uint32_t> bytes;
        std::vector<GfxShaderResource> resources;
        std::vector<GfxShaderVariable> variables;
    };

    class ShaderCompiler {
    public:
        virtual ShaderCompileResult compile(const ShaderCompileDesc& desc) = 0;
    };
}