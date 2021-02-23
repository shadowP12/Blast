#pragma once
#include "ShaderCompiler.h"

namespace Blast {
    class VulkanShaderCompiler : public ShaderCompiler {
    public:
        std::vector<uint32_t> compile(const ShaderCompileDesc& desc) override;
    };

}