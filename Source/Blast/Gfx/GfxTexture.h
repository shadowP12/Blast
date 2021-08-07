#pragma once
#include "GfxDefine.h"

namespace blast {
    // 1.应该由format去判断该纹理属于什么attachment
    struct GfxTextureDesc {
        uint32_t width;
        uint32_t height;
        uint32_t depth = 1;
        uint32_t num_mips = 1;
        uint32_t num_layers = 1;
        Format format;
        SampleCount sample_count = SAMPLE_COUNT_1;
        ResourceUsage usage;
        ResourceType type;
        ResourceState state;
    };

    class GfxTexture {
    public:
        GfxTexture(const GfxTextureDesc& desc);

        virtual ~GfxTexture() = default;

        uint32_t GetWidth() { return _width; }

        uint32_t GetHeight() {return _height; }

        uint32_t GetDepth() {return _depth; }

        Format GetFormat() { return _format; }

        SampleCount GetSampleCount() { return _sample_count; }

        ResourceType GetResourceType() { return _type; }

        ResourceState GetResourceState() { return _state; }

        void SetResourceState(ResourceState state) { _state = state; }

    protected:
        friend class GfxTextureView;
        uint32_t _width;
        uint32_t _height;
        uint32_t _depth;
        uint32_t _num_mips;
        uint32_t _num_layers;
        Format _format;
        SampleCount _sample_count;
        ResourceUsage _usage;
        ResourceType _type;
        ResourceState _state;
    };

    // TextureView决定Texture的呈现形式
    struct GfxTextureViewDesc {
        uint32_t level = 0;
        uint32_t layer = 0;
        GfxTexture* texture = nullptr;
    };

    class GfxTextureView {
    public:
        GfxTextureView(const GfxTextureViewDesc& desc);

        virtual ~GfxTextureView() = default;

        uint32_t GetLevel() { return _level; }

        uint32_t GetLayer() { return _layer; }

    protected:
        uint32_t _level;
        uint32_t _layer;
    };

}
