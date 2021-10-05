#pragma once
#include "GfxDefine.h"

// 合并RenderPass和Framebuffer

namespace blast {
    class GfxTextureView;

    struct GfxFramebufferDesc {
        uint32_t width;
        uint32_t height;
        uint32_t num_colors = 0;
        bool has_depth_stencil = false;
        GfxTextureView* colors[MAX_RENDER_TARGET_ATTACHMENTS];
        GfxTextureView* depth_stencil;
        SampleCount sample_count = SAMPLE_COUNT_1;
    };

    class GfxFramebuffer {
    public:GfxFramebuffer(const GfxFramebufferDesc &desc);

        virtual ~GfxFramebuffer() = default;

        uint32_t GetWidth() { return _width; }

        uint32_t GetHeight() { return _height; }

        uint32_t GetColorAttachmentCount() { return _num_colors; }

        GfxTextureView* GetColor(uint32_t index);

        bool HasDepthStencilAttachment() { return _has_depth_stencil; }

        GfxTextureView* GetDepthStencil();

        SampleCount GetSampleCount() { return _sample_count; }

    protected:
        uint32_t _width;
        uint32_t _height;
        uint32_t _num_colors;
        bool _has_depth_stencil;
        SampleCount _sample_count;
        GfxTextureView* _colors[MAX_RENDER_TARGET_ATTACHMENTS];
        GfxTextureView* _depth_stencil;
    };
}