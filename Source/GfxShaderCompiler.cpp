#include "GfxShaderCompiler.h"

// Vulkan
#include "Vulkan/VulkanShaderCompiler.h"

namespace blast {
    GfxShaderCompiler* GfxShaderCompiler::CreateShaderCompiler() {
        return new VulkanShaderCompiler();
    }
}