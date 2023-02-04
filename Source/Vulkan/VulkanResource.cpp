#include "VulkanResource.h"
#include "VulkanDevice.h"

namespace blast
{
VulkanSampler::VulkanSampler(blast::VulkanDevice* in_device)
    : GfxSampler()
{
    device = in_device;
}

VulkanSampler::~VulkanSampler()
{
    device->DestroySampler(this);
}

VulkanBuffer::VulkanBuffer(blast::VulkanDevice* in_device)
    : GfxBuffer()
{
    device = in_device;
}

VulkanBuffer::~VulkanBuffer()
{
    device->DestroyBuffer(this);
}

VulkanTexture::VulkanTexture(blast::VulkanDevice* in_device)
    : GfxTexture()
{
    device = in_device;
}

VulkanTexture::~VulkanTexture()
{
    device->DestroyTexture(this);
}

VulkanRenderPass::VulkanRenderPass(blast::VulkanDevice* in_device)
    : GfxRenderPass()
{
    device = in_device;
}

VulkanRenderPass::~VulkanRenderPass()
{
    device->DestroyRenderPass(this);
}

VulkanSwapChain::VulkanSwapChain(blast::VulkanDevice* in_device)
    : GfxSwapChain()
{
    device = in_device;
}

VulkanSwapChain::~VulkanSwapChain()
{
    device->DestroySwapChain(this);
}

VulkanShader::VulkanShader(blast::VulkanDevice* in_device)
    : GfxShader()
{
    device = in_device;
}

VulkanShader::~VulkanShader()
{
    device->DestroyShader(this);
}

VulkanPipeline::VulkanPipeline(blast::VulkanDevice* in_device)
    : GfxPipeline()
{
    device = in_device;
}

VulkanPipeline::~VulkanPipeline()
{
    device->DestroyPipeline(this);
}
}// namespace blast