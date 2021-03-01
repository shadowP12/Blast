#pragma once
#include "ShaderCompiler.h"

namespace Blast {
    class VulkanShaderCompiler : public ShaderCompiler {
    public:
        VulkanShaderCompiler();
        ~VulkanShaderCompiler();
        ShaderCompileResult compile(const ShaderCompileDesc& desc) override;
    };

}