#include "GfxShader.h"

namespace blast {
    GfxShader::GfxShader(const GfxShaderDesc &desc) {
        _stage = desc.stage;
    }
}