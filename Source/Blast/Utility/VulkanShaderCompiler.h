#pragma once
#include "ShaderCompiler.h"

namespace blast {
    class VulkanShaderCompiler : public ShaderCompiler {
    public:
        VulkanShaderCompiler();

        ~VulkanShaderCompiler();

        ShaderCompileResult Compile(const ShaderCompileDesc& desc) override;
    };

}