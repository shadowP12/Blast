#pragma once
#include "GfxDefine.h"
#include <string>
#include <vector>

namespace blast
{
enum ResourceType
{
    RESOURCE_TYPE_UNDEFINED = 0,
    RESOURCE_TYPE_SAMPLER = 0x01,
    RESOURCE_TYPE_TEXTURE = (RESOURCE_TYPE_SAMPLER << 1),
    RESOURCE_TYPE_RW_TEXTURE = (RESOURCE_TYPE_TEXTURE << 1),
    RESOURCE_TYPE_RW_BUFFER = (RESOURCE_TYPE_RW_TEXTURE << 1),
    RESOURCE_TYPE_UNIFORM_BUFFER = (RESOURCE_TYPE_RW_BUFFER << 1),
    RESOURCE_TYPE_VERTEX_BUFFER = (RESOURCE_TYPE_UNIFORM_BUFFER << 1),
    RESOURCE_TYPE_INDEX_BUFFER = (RESOURCE_TYPE_VERTEX_BUFFER << 1),
    RESOURCE_TYPE_INDIRECT_BUFFER = (RESOURCE_TYPE_INDEX_BUFFER << 1),
    RESOURCE_TYPE_TEXTURE_CUBE = (RESOURCE_TYPE_TEXTURE | (RESOURCE_TYPE_INDIRECT_BUFFER << 1)),
    RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER = (RESOURCE_TYPE_TEXTURE_CUBE << 1),
};

struct GfxShaderResource
{
    std::string name;
    uint32_t set;
    uint32_t reg;
    uint32_t size;
    ResourceType type;
    TextureDimension dim;
};

struct GfxShaderVariable
{
    std::string name;
    uint16_t parent_index;
    uint16_t offset;
    uint16_t size;
    uint16_t count;
    UniformType type;
};

struct ShaderCompileDesc
{
    std::string code;
    std::string preamble;
    std::vector<std::string> include_dirs;
    ShaderStage stage;
};

struct ShaderCompileResult
{
    bool success;
    std::vector<uint32_t> bytes;
    std::vector<GfxShaderResource> resources;
    std::vector<GfxShaderVariable> variables;
};

class GfxShaderCompiler
{
public:
    static GfxShaderCompiler* CreateShaderCompiler();

    virtual ShaderCompileResult Compile(const ShaderCompileDesc& desc) = 0;
};
}// namespace blast