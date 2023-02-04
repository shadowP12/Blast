#pragma once
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <vector>

#if WIN32
#include <windows.h>
#endif

#define BLAST_SAFE_DELETE(x) \
    {                        \
        if (x)               \
        {                    \
            delete x;        \
            x = nullptr;     \
        }                    \
    }

#define BLAST_SAFE_DELETE_ARRAY(x) \
    {                              \
        delete[] x;                \
        x = nullptr;               \
    }

#define BLAST_LOGE(...)                                 \
    do                                                  \
    {                                                   \
        fprintf(stderr, "[BLAST_ERROR]: " __VA_ARGS__); \
        fflush(stderr);                                 \
    } while (false)

#define BLAST_LOGW(...)                                \
    do                                                 \
    {                                                  \
        fprintf(stderr, "[BLAST_WARN]: " __VA_ARGS__); \
        fflush(stderr);                                \
    } while (false)

#define BLAST_LOGI(...)                                \
    do                                                 \
    {                                                  \
        fprintf(stderr, "[BLAST_INFO]: " __VA_ARGS__); \
        fflush(stderr);                                \
    } while (false)

#ifdef __cplusplus
#ifndef BLAST_MAKE_ENUM_FLAG
#define BLAST_MAKE_ENUM_FLAG(TYPE, ENUM_TYPE)                     \
    static inline ENUM_TYPE operator|(ENUM_TYPE a, ENUM_TYPE b)   \
    {                                                             \
        return (ENUM_TYPE)((TYPE)(a) | (TYPE)(b));                \
    }                                                             \
    static inline ENUM_TYPE operator&(ENUM_TYPE a, ENUM_TYPE b)   \
    {                                                             \
        return (ENUM_TYPE)((TYPE)(a) & (TYPE)(b));                \
    }                                                             \
    static inline ENUM_TYPE operator|=(ENUM_TYPE& a, ENUM_TYPE b) \
    {                                                             \
        return a = (a | b);                                       \
    }                                                             \
    static inline ENUM_TYPE operator&=(ENUM_TYPE& a, ENUM_TYPE b) \
    {                                                             \
        return a = (a & b);                                       \
    }
#endif
#else
#define MAKE_ENUM_FLAG(TYPE, ENUM_TYPE)
#endif

#define MAX_VERTEX_BINDINGS 15
#define MAX_VERTEX_ATTRIBS 15
#define MAX_RENDER_TARGET_ATTACHMENTS 4

#define BLAST_CMD_COUNT 8
#define BLAST_QUEUE_COUNT 2
#define BLAST_BUFFER_COUNT 2
#define BLAST_VIEWPORT_COUNT 1
#define BLAST_SCISSOR_COUNT 1

namespace blast
{
// hash
template<class T>
constexpr void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

enum BlendOp
{
    BLEND_OP_ADD,
    BLEND_OP_SUBTRACT,
    BLEND_OP_REV_SUBTRACT,
    BLEND_OP_MIN,
    BLEND_OP_MAX,
};

enum BlendConstant
{
    BLEND_ZERO = 0,
    BLEND_ONE,
    BLEND_SRC_COLOR,
    BLEND_ONE_MINUS_SRC_COLOR,
    BLEND_DST_COLOR,
    BLEND_ONE_MINUS_DST_COLOR,
    BLEND_SRC_ALPHA,
    BLEND_ONE_MINUS_SRC_ALPHA,
    BLEND_DST_ALPHA,
    BLEND_ONE_MINUS_DST_ALPHA,
    BLEND_SRC_ALPHA_SATURATE,
    BLEND_BLEND_FACTOR,
    BLEND_ONE_MINUS_BLEND_FACTOR,
    MAX_BLEND_CONSTANTS
};

enum StencilOp
{
    STENCIL_OP_KEEP,
    STENCIL_OP_SET_ZERO,
    STENCIL_OP_REPLACE,
    STENCIL_OP_INVERT,
    STENCIL_OP_INCR,
    STENCIL_OP_DECR,
    STENCIL_OP_INCR_SAT,
    STENCIL_OP_DECR_SAT,
    MAX_STENCIL_OPS,
};

enum CompareMode
{
    COMPARE_NEVER,
    COMPARE_LESS,
    COMPARE_EQUAL,
    COMPARE_LEQUAL,
    COMPARE_GREATER,
    COMPARE_NOTEQUAL,
    COMPARE_GEQUAL,
    COMPARE_ALWAYS,
    MAX_COMPARE_MODES,
};

enum ColorWriteEnable
{
    COLOR_WRITE_DISABLE = 0,
    COLOR_WRITE_ENABLE_RED = 1,
    COLOR_WRITE_ENABLE_GREEN = 2,
    COLOR_WRITE_ENABLE_BLUE = 4,
    COLOR_WRITE_ENABLE_ALPHA = 8,
    COLOR_WRITE_ENABLE_ALL = (((COLOR_WRITE_ENABLE_RED | COLOR_WRITE_ENABLE_GREEN) | COLOR_WRITE_ENABLE_BLUE) | COLOR_WRITE_ENABLE_ALPHA)
};

enum FillMode
{
    FILL_SOLID,
    FILL_WIREFRAME,
    MAX_FILL_MODES
};

enum CullMode
{
    CULL_NONE = 0,
    CULL_BACK,
    CULL_FRONT,
    CULL_BOTH,
    MAX_CULL_MODES
};

enum FrontFace
{
    FRONT_FACE_CCW = 0,
    FRONT_FACE_CW
};

enum FilterType
{
    FILTER_NEAREST = 0,
    FILTER_LINEAR,
};

enum AddressMode
{
    ADDRESS_MODE_MIRROR,
    ADDRESS_MODE_REPEAT,
    ADDRESS_MODE_CLAMP_TO_EDGE,
    ADDRESS_MODE_CLAMP_TO_BORDER
};

enum MipmapMode
{
    MIPMAP_MODE_NEAREST = 0,
    MIPMAP_MODE_LINEAR
};

enum BlendStateTargets
{
    BLEND_STATE_TARGET_0 = 0x1,
    BLEND_STATE_TARGET_1 = 0x2,
    BLEND_STATE_TARGET_2 = 0x4,
    BLEND_STATE_TARGET_3 = 0x8,
    BLEND_STATE_TARGET_4 = 0x10,
    BLEND_STATE_TARGET_5 = 0x20,
    BLEND_STATE_TARGET_6 = 0x40,
    BLEND_STATE_TARGET_7 = 0x80,
    BLEND_STATE_TARGET_ALL = 0xFF,
};
BLAST_MAKE_ENUM_FLAG(uint32_t, BlendStateTargets)

enum SampleCount
{
    SAMPLE_COUNT_1 = 1,
    SAMPLE_COUNT_2 = 2,
    SAMPLE_COUNT_4 = 4,
    SAMPLE_COUNT_8 = 8,
    SAMPLE_COUNT_16 = 16,
};

enum Format
{
    FORMAT_UNKNOWN,

    FORMAT_R32G32B32A32_FLOAT,
    FORMAT_R32G32B32A32_UINT,
    FORMAT_R32G32B32A32_SINT,

    FORMAT_R32G32B32_FLOAT,
    FORMAT_R32G32B32_UINT,
    FORMAT_R32G32B32_SINT,

    FORMAT_R16G16B16A16_FLOAT,
    FORMAT_R16G16B16A16_UNORM,
    FORMAT_R16G16B16A16_UINT,
    FORMAT_R16G16B16A16_SNORM,
    FORMAT_R16G16B16A16_SINT,

    FORMAT_R32G32_FLOAT,
    FORMAT_R32G32_UINT,
    FORMAT_R32G32_SINT,
    FORMAT_R32G8X24_TYPELESS,
    FORMAT_D32_FLOAT_S8X24_UINT,

    FORMAT_R10G10B10A2_UNORM,
    FORMAT_R10G10B10A2_UINT,
    FORMAT_R11G11B10_FLOAT,
    FORMAT_R8G8B8A8_UNORM,
    FORMAT_R8G8B8A8_UNORM_SRGB,
    FORMAT_R8G8B8A8_UINT,
    FORMAT_R8G8B8A8_SNORM,
    FORMAT_R8G8B8A8_SINT,
    FORMAT_B8G8R8A8_UNORM,
    FORMAT_B8G8R8A8_UNORM_SRGB,
    FORMAT_R16G16_FLOAT,
    FORMAT_R16G16_UNORM,
    FORMAT_R16G16_UINT,
    FORMAT_R16G16_SNORM,
    FORMAT_R16G16_SINT,
    FORMAT_R32_TYPELESS,
    FORMAT_D32_FLOAT,
    FORMAT_R32_FLOAT,
    FORMAT_R32_UINT,
    FORMAT_R32_SINT,
    FORMAT_R24G8_TYPELESS,
    FORMAT_D24_UNORM_S8_UINT,

    FORMAT_R8G8_UNORM,
    FORMAT_R8G8_UINT,
    FORMAT_R8G8_SNORM,
    FORMAT_R8G8_SINT,
    FORMAT_R16_TYPELESS,
    FORMAT_R16_FLOAT,
    FORMAT_D16_UNORM,
    FORMAT_R16_UNORM,
    FORMAT_R16_UINT,
    FORMAT_R16_SNORM,
    FORMAT_R16_SINT,

    FORMAT_R8_UNORM,
    FORMAT_R8_UINT,
    FORMAT_R8_SNORM,
    FORMAT_R8_SINT,

    FORMAT_BC1_UNORM,
    FORMAT_BC1_UNORM_SRGB,
    FORMAT_BC2_UNORM,
    FORMAT_BC2_UNORM_SRGB,
    FORMAT_BC3_UNORM,
    FORMAT_BC3_UNORM_SRGB,
    FORMAT_BC4_UNORM,
    FORMAT_BC4_SNORM,
    FORMAT_BC5_UNORM,
    FORMAT_BC5_SNORM,
    FORMAT_BC6H_UF16,
    FORMAT_BC6H_SF16,
    FORMAT_BC7_UNORM,
    FORMAT_BC7_UNORM_SRGB,

    FORMAT_X8_D24_UNORM_PACK32,
    FORMAT_S8_UINT,
    FORMAT_D16_UNORM_S8_UINT,
    FORMAT_D32_FLOAT_S8_UINT
};

enum MemoryUsage
{
    MEMORY_USAGE_GPU_ONLY = 0,
    MEMORY_USAGE_CPU_TO_GPU = 1,
    MEMORY_USAGE_GPU_TO_CPU = 2
};

enum ResourceUsage
{
    RESOURCE_USAGE_UNDEFINED = 0,
    RESOURCE_USAGE_UNIFORM_BUFFER = 1 << 0,
    RESOURCE_USAGE_VERTEX_BUFFER = 1 << 1,
    RESOURCE_USAGE_INDEX_BUFFER = 1 << 2,
    RESOURCE_USAGE_INDIRECT_BUFFER = 1 << 3,
    RESOURCE_USAGE_RW_BUFFER = 1 << 4,
    RESOURCE_USAGE_CUBE_TEXTURE = 1 << 5,
    RESOURCE_USAGE_RENDER_TARGET = 1 << 6,
    RESOURCE_USAGE_DEPTH_STENCIL = 1 << 7,
    RESOURCE_USAGE_SHADER_RESOURCE = 1 << 8,
    RESOURCE_USAGE_UNORDERED_ACCESS = 1 << 9
};
BLAST_MAKE_ENUM_FLAG(uint32_t, ResourceUsage)

enum ResourceState
{
    RESOURCE_STATE_UNDEFINED = 0,
    RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
    RESOURCE_STATE_INDEX_BUFFER = 0x2,
    RESOURCE_STATE_RENDERTARGET = 0x4,
    RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
    RESOURCE_STATE_DEPTH_WRITE = 0x10,
    RESOURCE_STATE_DEPTH_READ = 0x20,
    RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
    RESOURCE_STATE_SHADER_RESOURCE = 0x40 | 0x80,
    RESOURCE_STATE_STREAM_OUT = 0x100,
    RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
    RESOURCE_STATE_COPY_DEST = 0x400,
    RESOURCE_STATE_COPY_SOURCE = 0x800,
    RESOURCE_STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
    RESOURCE_STATE_PRESENT = 0x1000,
    RESOURCE_STATE_COMMON = 0x2000,
    RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x4000,
};
BLAST_MAKE_ENUM_FLAG(uint32_t, ResourceState)

enum LoadAction
{
    LOAD_DONTCARE,
    LOAD_LOAD,
    LOAD_CLEAR
};

enum StoreAction
{
    STORE_STORE,
    STORE_DONTCARE
};

enum QueueType
{
    QUEUE_GRAPHICS = 0,
    QUEUE_COMPUTE = 1,
    QUEUE_COPY = 2
};

enum ShaderStage
{
    SHADER_STAGE_NONE = 0,
    SHADER_STAGE_VERT = 0X00000001,
    SHADER_STAGE_TESC = 0X00000002,
    SHADER_STAGE_TESE = 0X00000004,
    SHADER_STAGE_GEOM = 0X00000008,
    SHADER_STAGE_FRAG = 0X00000010,
    SHADER_STAGE_COMP = 0X00000020,
    SHADER_STAGE_RAYTRACING = 0X00000040,
    SHADER_STAGE_ALL_GRAPHICS = ((uint32_t)SHADER_STAGE_VERT | (uint32_t)SHADER_STAGE_TESC | (uint32_t)SHADER_STAGE_TESE | (uint32_t)SHADER_STAGE_GEOM | (uint32_t)SHADER_STAGE_FRAG),
    SHADER_STAGE_HULL = SHADER_STAGE_TESC,
    SHADER_STAGE_DOMN = SHADER_STAGE_TESE,
    SHADER_STAGE_COUNT = 7,
};
BLAST_MAKE_ENUM_FLAG(uint32_t, ShaderStage)

enum TextureDimension
{
    TEXTURE_DIM_1D,
    TEXTURE_DIM_2D,
    TEXTURE_DIM_2DMS,
    TEXTURE_DIM_3D,
    TEXTURE_DIM_CUBE,
    TEXTURE_DIM_1D_ARRAY,
    TEXTURE_DIM_2D_ARRAY,
    TEXTURE_DIM_2DMS_ARRAY,
    TEXTURE_DIM_CUBE_ARRAY,
    TEXTURE_DIM_COUNT,
    TEXTURE_DIM_UNDEFINED,
};

enum UniformType
{
    UNIFORM_UNDEFINED,
    UNIFORM_BOOL,
    UNIFORM_FLOAT,
    UNIFORM_FLOAT2,
    UNIFORM_FLOAT3,
    UNIFORM_FLOAT4,
    UNIFORM_INT,
    UNIFORM_INT2,
    UNIFORM_INT3,
    UNIFORM_INT4,
    UNIFORM_UINT,
    UNIFORM_UINT2,
    UNIFORM_UINT3,
    UNIFORM_UINT4,
    UNIFORM_MAT4
};

enum VertexAttribRate
{
    VERTEX_ATTRIB_RATE_VERTEX = 0,
    VERTEX_ATTRIB_RATE_INSTANCE = 1,
    VERTEX_ATTRIB_RATE_COUNT,
};

enum PrimitiveTopology
{
    PRIMITIVE_TOPO_POINT_LIST = 0,
    PRIMITIVE_TOPO_LINE_LIST,
    PRIMITIVE_TOPO_LINE_STRIP,
    PRIMITIVE_TOPO_TRI_LIST,
    PRIMITIVE_TOPO_TRI_STRIP,
    PRIMITIVE_TOPO_PATCH_LIST,
    PRIMITIVE_TOPO_COUNT,
};

enum IndexType
{
    INDEX_TYPE_UINT32 = 0,
    INDEX_TYPE_UINT16,
};

enum ShaderSemantic
{
    SEMANTIC_UNDEFINED = 1 << 0,
    SEMANTIC_POSITION = 1 << 1,
    SEMANTIC_NORMAL = 1 << 2,
    SEMANTIC_COLOR = 1 << 3,
    SEMANTIC_TANGENT = 1 << 4,
    SEMANTIC_BITANGENT = 1 << 5,
    SEMANTIC_JOINTS = 1 << 6,
    SEMANTIC_WEIGHTS = 1 << 7,
    SEMANTIC_TEXCOORD0 = 1 << 8,
    SEMANTIC_TEXCOORD1 = 1 << 9,
    SEMANTIC_CUSTOM0 = 1 << 10,
    SEMANTIC_CUSTOM1 = 1 << 11,
    SEMANTIC_CUSTOM2 = 1 << 12,
    SEMANTIC_CUSTOM3 = 1 << 13,
    SEMANTIC_CUSTOM4 = 1 << 14,
    SEMANTIC_CUSTOM5 = 1 << 15
};
BLAST_MAKE_ENUM_FLAG(uint32_t, ShaderSemantic)

enum FenceStatus
{
    FENCE_STATUS_COMPLETE = 0,
    FENCE_STATUS_INCOMPLETE,
    FENCE_STATUS_NOTSUBMITTED,
};

enum PipelineType
{
    PIPELINE_TYPE_GRAPHICS = 0,
    PIPELINE_TYPE_COMPUTE,
    MAX_PIPELINE_TYPE
};

enum ClearFlag
{
    CLEAR_NONE = 0,
    CLEAR_COLOR = 0X00000001,
    CLEAR_DEPTH = 0X00000002,
    CLEAR_STENCIL = 0X00000004,
    CLEAR_ALL = ((uint32_t)CLEAR_COLOR | (uint32_t)CLEAR_DEPTH | (uint32_t)CLEAR_STENCIL)
};
BLAST_MAKE_ENUM_FLAG(uint32_t, ClearFlag)

enum ColorComponentFlag
{
    COLOR_COMPONENT_NONE = 0x00000000,
    COLOR_COMPONENT_R = 0x00000001,
    COLOR_COMPONENT_G = 0x00000002,
    COLOR_COMPONENT_B = 0x00000004,
    COLOR_COMPONENT_A = 0x00000008,
    COLOR_COMPONENT_ALL = ((uint32_t)COLOR_COMPONENT_R | (uint32_t)COLOR_COMPONENT_G | (uint32_t)COLOR_COMPONENT_B) | (uint32_t)COLOR_COMPONENT_A
};
BLAST_MAKE_ENUM_FLAG(uint32_t, ColorComponentFlag)

enum SubResourceType
{
    // Shader resource view
    SRV = 0,
    // Unordered access view
    UAV,
    // Render target view
    RTV,
    // Depth stencil view
    DSV
};

struct ClearValue
{
    float color[4];
    struct ClearDepthStencil
    {
        float depth;
        uint32_t stencil;
    } depthstencil;
};

struct GfxSamplerDesc
{
    FilterType min_filter = FILTER_LINEAR;
    FilterType mag_filter = FILTER_LINEAR;
    AddressMode address_u = ADDRESS_MODE_REPEAT;
    AddressMode address_v = ADDRESS_MODE_REPEAT;
    AddressMode address_w = ADDRESS_MODE_REPEAT;
    MipmapMode mipmap_mode = MIPMAP_MODE_LINEAR;
};

class GfxSampler
{
public:
    GfxSampler() = default;

    virtual ~GfxSampler() = default;

    const GfxSamplerDesc& GetDesc() const
    {
        return desc;
    }

public:
    GfxSamplerDesc desc;
};

class GfxResource
{
public:
    enum class ResourceType
    {
        BUFFER,
        TEXTURE,
        UNKNOWN_TYPE,
    };

    GfxResource() = default;

    virtual ~GfxResource() = default;

    virtual ResourceType GetType()
    {
        return ResourceType::UNKNOWN_TYPE;
    }

protected:
    MemoryUsage mem_usage;
    ResourceUsage res_usage;
    ResourceState res_state;
};

struct GfxBufferDesc
{
    uint32_t size;
    MemoryUsage mem_usage;
    ResourceUsage res_usage;
};

class GfxBuffer : public GfxResource
{
public:
    GfxBuffer() = default;

    virtual ~GfxBuffer() = default;

    virtual ResourceType GetType() override
    {
        return ResourceType::BUFFER;
    }

public:
    uint32_t size{};
    MemoryUsage mem_usage;
    ResourceUsage res_usage;
};

struct GfxTextureDesc
{
    uint32_t width;
    uint32_t height;
    uint32_t depth = 1;
    uint32_t num_layers = 1;
    uint32_t num_levels = 1;
    Format format;
    ClearValue clear = {};
    SampleCount sample_count = SAMPLE_COUNT_1;
    ResourceState state;
    MemoryUsage mem_usage;
    ResourceUsage res_usage;
};

class GfxTexture : public GfxResource
{
public:
    GfxTexture() = default;

    virtual ~GfxTexture() = default;

    virtual ResourceType GetType() override
    {
        return ResourceType::TEXTURE;
    }

public:
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t num_layers;
    uint32_t num_levels;
    Format format;
    ClearValue clear;
    SampleCount sample_count;
    ResourceState state;
    MemoryUsage mem_usage;
    ResourceUsage res_usage;
};

struct RenderPassAttachment
{
    enum AttachmentType
    {
        RENDERTARGET,
        DEPTH_STENCIL,
        RESOLVE
    } type = RENDERTARGET;

    LoadAction loadop = LOAD_LOAD;
    StoreAction storeop = STORE_STORE;

    const GfxTexture* texture = nullptr;
    int32_t subresource = -1;

    static RenderPassAttachment RenderTarget(
        const GfxTexture* resource = nullptr,
        int32_t subresource = -1,
        LoadAction load_op = LOAD_LOAD,
        StoreAction store_op = STORE_STORE)
    {
        RenderPassAttachment attachment;
        attachment.type = RENDERTARGET;
        attachment.texture = resource;
        attachment.subresource = subresource;
        attachment.loadop = load_op;
        attachment.storeop = store_op;
        return attachment;
    }

    static RenderPassAttachment DepthStencil(
        const GfxTexture* resource = nullptr,
        int32_t subresource = -1,
        LoadAction load_op = LOAD_LOAD,
        StoreAction store_op = STORE_STORE)
    {
        RenderPassAttachment attachment;
        attachment.type = DEPTH_STENCIL;
        attachment.texture = resource;
        attachment.subresource = subresource;
        attachment.loadop = load_op;
        attachment.storeop = store_op;
        return attachment;
    }

    static RenderPassAttachment Resolve(
        const GfxTexture* resource = nullptr)
    {
        RenderPassAttachment attachment;
        attachment.type = RESOLVE;
        attachment.texture = resource;
        return attachment;
    }
};
struct GfxRenderPassDesc
{
    std::vector<RenderPassAttachment> attachments;
};

class GfxRenderPass
{
public:
    GfxRenderPass() = default;

    virtual ~GfxRenderPass() = default;

    const GfxRenderPassDesc& GetDesc() const
    {
        return desc;
    }

public:
    GfxRenderPassDesc desc;
    size_t hash = 0;
};

struct GfxSwapChainDesc
{
    void* window = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t buffer_count = 2;
    Format format;
    bool fulls_creen = false;
    bool vsync = true;
    float clear_color[4] = {0, 0, 0, 1};
};

class GfxSwapChain
{
public:
    GfxSwapChain() = default;

    virtual ~GfxSwapChain() = default;

    const GfxSwapChainDesc& GetDesc() const
    {
        return desc;
    }

public:
    GfxSwapChainDesc desc;
};

struct GfxShaderDesc
{
    void* bytecode = nullptr;
    uint32_t bytecode_length = 0;
    ShaderStage stage;
};

class GfxShader
{
public:
    GfxShader() = default;

    virtual ~GfxShader() = default;

    ShaderStage GetStage()
    {
        return stage;
    }

public:
    ShaderStage stage;
};

struct GfxInputLayout
{
    struct Element
    {
        uint32_t binding = 0;
        uint32_t location = 0;
        uint32_t offset = 0;
        uint32_t size = 0;
        Format format = FORMAT_UNKNOWN;
        ShaderSemantic semantic = SEMANTIC_UNDEFINED;
        VertexAttribRate rate = VERTEX_ATTRIB_RATE_VERTEX;
    };
    std::vector<Element> elements;
};

struct GfxBlendState
{
    bool alpha_to_coverage_eEnable = false;
    bool independent_blend_enable = false;

    struct RenderTargetBlendState
    {
        bool blend_enable = false;
        BlendConstant src_factor = BLEND_SRC_ALPHA;
        BlendConstant dst_factor = BLEND_ONE_MINUS_SRC_ALPHA;
        BlendOp blend_op = BLEND_OP_ADD;
        BlendConstant src_factor_alpha = BLEND_ONE;
        BlendConstant dst_factor_alpha = BLEND_ONE;
        BlendOp blend_op_alpha = BLEND_OP_ADD;
        ColorComponentFlag render_target_write_mask = COLOR_COMPONENT_ALL;
    };
    RenderTargetBlendState rt[8];
};

struct GfxDepthStencilState
{
    bool depth_test = false;
    bool depth_write = true;
    CompareMode depth_func = COMPARE_LEQUAL;
    bool stencil_test = false;
    uint8_t stencil_read_mask = 0xff;
    uint8_t stencil_write_mask = 0xff;

    struct DepthStencilOp
    {
        StencilOp stencil_fail_op = STENCIL_OP_KEEP;
        StencilOp stencil_depth_fail_op = STENCIL_OP_KEEP;
        StencilOp stencil_pass_op = STENCIL_OP_KEEP;
        CompareMode stencil_func = COMPARE_ALWAYS;
    };
    DepthStencilOp front_face;
    DepthStencilOp back_face;
};

struct GfxRasterizerState
{
    int32_t depth_bias = 0;
    float slope_scaled_depth_bias = 0.0;
    FillMode fill_mode = FILL_SOLID;
    FrontFace front_face = FRONT_FACE_CCW;
    CullMode cull_mode = CULL_NONE;
};

struct GfxPipelineDesc
{
    GfxShader* vs = nullptr;
    GfxShader* fs = nullptr;
    GfxShader* hs = nullptr;
    GfxShader* ds = nullptr;
    GfxShader* gs = nullptr;
    GfxBlendState* bs = nullptr;
    GfxRasterizerState* rs = nullptr;
    GfxDepthStencilState* dss = nullptr;
    GfxInputLayout* il = nullptr;
    GfxRenderPass* rp = nullptr;
    GfxSwapChain* sc = nullptr;
    uint32_t patch_control_points = 3;
    SampleCount sample_count = SAMPLE_COUNT_1;
    PrimitiveTopology primitive_topo = PRIMITIVE_TOPO_TRI_LIST;
};

class GfxPipeline
{
public:
    GfxPipeline() = default;

    virtual ~GfxPipeline() = default;

    const GfxPipelineDesc& GetDesc() const
    {
        return desc;
    }

public:
    GfxPipelineDesc desc;
};

class GfxCommandBuffer
{
public:
    GfxCommandBuffer() = default;

    virtual ~GfxCommandBuffer() = default;
};

struct GfxBufferCopyRange
{
    uint32_t src_offset;
    uint32_t dst_offset;
    uint32_t size;
    GfxBuffer* src_buffer;
    GfxBuffer* dst_buffer;
};

struct GfxImageCopyRange
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;
    uint32_t src_level = 0;
    uint32_t src_layer = 0;
    uint32_t dst_level = 0;
    uint32_t dst_layer = 0;
    GfxTexture* src_texture;
    GfxTexture* dst_texture;
};

struct GfxBufferImageCopyRange
{
    uint32_t level = 0;
    uint32_t layer = 0;
    uint32_t buffer_offset = 0;
    GfxBuffer* src_buffer = nullptr;
    GfxTexture* dst_texture = nullptr;
};

struct GfxResourceBarrier
{
    GfxResource* resource = nullptr;
    ResourceState new_state = RESOURCE_STATE_COMMON;
};

static const uint32_t BLAST_CBV_COUNT = 15;
static const uint32_t BLAST_SRV_COUNT = 64;
static const uint32_t BLAST_UAV_COUNT = 16;
static const uint32_t BLAST_SAMPLER_COUNT = 16;
struct GfxBindingTable
{
    GfxBuffer* cbv[BLAST_CBV_COUNT];
    uint64_t cbv_size[BLAST_CBV_COUNT];
    uint64_t cbv_offset[BLAST_CBV_COUNT];
    GfxResource* srv[BLAST_SRV_COUNT];
    uint32_t srv_index[BLAST_SRV_COUNT] = {};
    GfxResource* uav[BLAST_UAV_COUNT];
    uint32_t uav_index[BLAST_UAV_COUNT] = {};
    GfxSampler* sam[BLAST_SAMPLER_COUNT];
};

uint32_t GetFormatStride(Format format);

bool IsFormatStencilSupport(Format format);

inline constexpr uint32_t GetNextPowerOfTwo(uint32_t x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

inline constexpr uint64_t GetNextPowerOfTwo(uint64_t x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32u;
    return ++x;
}

inline constexpr uint32_t AlignTo(uint32_t value, uint32_t alignment)
{
    return ((value + alignment - 1) / alignment) * alignment;
}

inline constexpr uint64_t AlignTo(uint64_t value, uint64_t alignment)
{
    return ((value + alignment - 1) / alignment) * alignment;
}
}// namespace blast
