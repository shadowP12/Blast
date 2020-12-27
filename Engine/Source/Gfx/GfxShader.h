#pragma once
#include "GfxDefine.h"
#include <string>
#include <vector>

namespace Blast {

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
    };

    struct GfxShaderReflection {
        ShaderStage stage;
        std::vector<GfxShaderResource> resources;
        std::vector<GfxShaderVariable> variables;
    };

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