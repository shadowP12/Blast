#pragma once
#include "../GfxShaderCompiler.h"

namespace blast
{
class VulkanShaderCompiler : public GfxShaderCompiler
{
public:
    VulkanShaderCompiler();

    ~VulkanShaderCompiler();

    ShaderCompileResult Compile(const ShaderCompileDesc& desc) override;
};

}// namespace blast