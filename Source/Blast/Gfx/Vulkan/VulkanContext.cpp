#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"
#include "VulkanSwapchain.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include <vector>

namespace blast {
    static bool IsLayerSupported(const char *required, const std::vector<VkLayerProperties> &available) {
        for (const VkLayerProperties &availableLayer : available) {
            if (strcmp(availableLayer.layerName, required) == 0) {
                return true;
            }
        }
        return false;
    }

    static bool IsExtensionSupported(const char *required, const std::vector<VkExtensionProperties> &available) {
        for (const VkExtensionProperties &available_extension : available) {
            if (strcmp(available_extension.extensionName, required) == 0) {
                return true;
            }
        }
        return false;
    }

#if VULKAN_DEBUG
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                               VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                               const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                               void* user_data) {
        if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            BLAST_LOGW("[Vulkan]: Validation Error %s: %s\n", callback_data->pMessageIdName, callback_data->pMessage);
        } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            BLAST_LOGE("[Vulkan]: Validation Warning %s: %s\n", callback_data->pMessageIdName, callback_data->pMessage);
        }
        return VK_FALSE;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags,
                                                       VkDebugReportObjectTypeEXT type,
                                                       uint64_t object,
                                                       size_t location,
                                                       int32_t message_code,
                                                       const char* layer_prefix,
                                                       const char* message,
                                                       void* user_data) {
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            BLAST_LOGE("[Vulkan]: Validation Error: %s: %s\n", layer_prefix, message);
        } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
            BLAST_LOGW("[Vulkan]: Validation Warning: %s: %s\n", layer_prefix, message);
        } else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
            BLAST_LOGI("[Vulkan]: Validation PerfWarning: %s: %s\n", layer_prefix, message);
        } else {
            BLAST_LOGI("[Vulkan]: Validation Info: %s: %s\n", layer_prefix, message);
        }
        return VK_FALSE;
    }
#endif

    VulkanContext::VulkanContext() {
        if (volkInitialize() != VK_SUCCESS) {
            return;
        }
        uint32_t num_instance_available_layers;
        VK_ASSERT(vkEnumerateInstanceLayerProperties(&num_instance_available_layers, nullptr));
        std::vector<VkLayerProperties> instance_supported_layers(num_instance_available_layers);
        VK_ASSERT(vkEnumerateInstanceLayerProperties(&num_instance_available_layers, instance_supported_layers.data()));

        uint32_t num_instance_available_extensions;
        VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &num_instance_available_extensions, nullptr));
        std::vector<VkExtensionProperties> instance_supported_extensions(num_instance_available_extensions);
        VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &num_instance_available_extensions, instance_supported_extensions.data()));

        std::vector<const char*> instance_required_layers;
        std::vector<const char*> instance_required_extensions;
        std::vector<const char*> instance_layers;
        std::vector<const char*> instance_extensions;

#if VULKAN_DEBUG
        if (IsLayerSupported("VK_LAYER_KHRONOS_validation", instance_supported_layers)) {
            instance_required_layers.push_back("VK_LAYER_KHRONOS_validation");
        } else {
            instance_required_layers.push_back("VK_LAYER_LUNARG_standard_validation");
        }
        bool debug_utils = false;
        if (IsExtensionSupported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, instance_supported_extensions)) {
            debug_utils = true;
            instance_required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        } else {
            instance_required_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
#endif
        instance_required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        instance_required_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

        for (auto it = instance_required_layers.begin(); it != instance_required_layers.end(); ++it) {
            if (IsLayerSupported(*it, instance_supported_layers)) {
                instance_layers.push_back(*it);
            }
        }

        for (auto it = instance_required_extensions.begin(); it != instance_required_extensions.end(); ++it) {
            if (IsExtensionSupported(*it, instance_supported_extensions)) {
                instance_extensions.push_back(*it);
            }
        }

        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pEngineName = "BlastEngine";
        app_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo ici;
        ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        ici.pNext = nullptr;
        ici.flags = 0;
        ici.pApplicationInfo = &app_info;
        ici.enabledLayerCount = instance_layers.size();
        ici.ppEnabledLayerNames = instance_layers.data();
        ici.enabledExtensionCount = instance_extensions.size();
        ici.ppEnabledExtensionNames = instance_extensions.data();

        VK_ASSERT(vkCreateInstance(&ici, nullptr, &_instance));

        volkLoadInstance(_instance);

#if VULKAN_DEBUG
        if (debug_utils) {
            VkDebugUtilsMessengerCreateInfoEXT dumci{VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
            dumci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            dumci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            dumci.pfnUserCallback = DebugUtilsMessengerCallback;

            VK_ASSERT(vkCreateDebugUtilsMessengerEXT(_instance, &dumci, nullptr, &_debug_messenger));

        } else {
            VkDebugReportCallbackCreateInfoEXT drcci{VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT};
            drcci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            drcci.pfnCallback = DebugReportCallback;

            VK_ASSERT(vkCreateDebugReportCallbackEXT(_instance, &drcci, nullptr, &_debug_report_callback));
        }
#endif
        uint32_t num_gpus = 0;
        VK_ASSERT(vkEnumeratePhysicalDevices(_instance, &num_gpus, nullptr));

        std::vector<VkPhysicalDevice> gpus(num_gpus);
        VK_ASSERT(vkEnumeratePhysicalDevices(_instance, &num_gpus, gpus.data()));

        for (auto &g : gpus) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(g, &props);
            BLAST_LOGI("Found Vulkan GPU: %s\n", props.deviceName);
            BLAST_LOGI("API: %u.%u.%u\n",
                   VK_VERSION_MAJOR(props.apiVersion),
                   VK_VERSION_MINOR(props.apiVersion),
                   VK_VERSION_PATCH(props.apiVersion));
            BLAST_LOGI("Driver: %u.%u.%u\n",
                   VK_VERSION_MAJOR(props.driverVersion),
                   VK_VERSION_MINOR(props.driverVersion),
                   VK_VERSION_PATCH(props.driverVersion));
        }

        // todo: 支持多个GPU选择
        _phy_device = gpus.front();

        vkGetPhysicalDeviceProperties(_phy_device, &_phy_device_properties);
        vkGetPhysicalDeviceFeatures(_phy_device, &_phy_device_features);
        vkGetPhysicalDeviceMemoryProperties(_phy_device, &_phy_device_memory_properties);

        uint32_t num_queue_families;
        vkGetPhysicalDeviceQueueFamilyProperties(_phy_device, &num_queue_families, nullptr);

        std::vector<VkQueueFamilyProperties> queue_Family_properties(num_queue_families);
        vkGetPhysicalDeviceQueueFamilyProperties(_phy_device, &num_queue_families, queue_Family_properties.data());

        uint32_t graphics_family = -1;
        uint32_t compute_family = -1;
        uint32_t transfer_family = -1;
        for (uint32_t i = 0; i < (uint32_t)queue_Family_properties.size(); i++) {
            if ((queue_Family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && (queue_Family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
                compute_family = i;
                break;
            }
        }

        for (uint32_t i = 0; i < (uint32_t)queue_Family_properties.size(); i++) {
            if ((queue_Family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                ((queue_Family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
                ((queue_Family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) {
                transfer_family = i;
                break;
            }
        }

        for (uint32_t i = 0; i < (uint32_t)queue_Family_properties.size(); i++) {
            if (queue_Family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphics_family = i;
                break;
            }
        }

        const float graphics_queue_prio = 0.0f;
        const float compute_queue_prio = 0.1f;
        const float transfer_queue_prio = 0.2f;

        // note:目前只使用三个queue
        std::vector<VkDeviceQueueCreateInfo> qci{};
        qci.resize(3);

        qci[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qci[0].queueFamilyIndex = graphics_family;
        qci[0].queueCount = 1;//queue_Family_properties[mGraphicsFamily].queueCount;
        qci[0].pQueuePriorities = &graphics_queue_prio;

        qci[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qci[1].queueFamilyIndex = compute_family;
        qci[1].queueCount = 1;//queue_Family_properties[mComputeFamily].queueCount;
        qci[1].pQueuePriorities = &compute_queue_prio;

        qci[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qci[2].queueFamilyIndex = transfer_family;
        qci[2].queueCount = 1;//queue_Family_properties[mTransferFamily].queueCount;
        qci[2].pQueuePriorities = &transfer_queue_prio;

        uint32_t num_device_available_extensions = 0;
        VK_ASSERT(vkEnumerateDeviceExtensionProperties(_phy_device, nullptr, &num_device_available_extensions, nullptr));

        std::vector<VkExtensionProperties> device_available_extensions(num_device_available_extensions);
        VK_ASSERT(vkEnumerateDeviceExtensionProperties(_phy_device, nullptr, &num_device_available_extensions, device_available_extensions.data()));

        std::vector<const char*> device_required_extensions;
        std::vector<const char*> device_extensions;
        device_required_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
        device_required_extensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);

        for (auto it = device_required_extensions.begin(); it != device_required_extensions.end(); ++it) {
            if (IsExtensionSupported(*it, device_available_extensions)) {
                device_extensions.push_back(*it);
            }
        }

        VkDeviceCreateInfo dci;
        dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        dci.pNext = nullptr;
        dci.flags = 0;
        dci.queueCreateInfoCount = qci.size();
        dci.pQueueCreateInfos = qci.data();
        dci.pEnabledFeatures = &_phy_device_features;
        dci.enabledExtensionCount = device_extensions.size();
        dci.ppEnabledExtensionNames = device_extensions.data();
        dci.enabledLayerCount = 0;
        dci.ppEnabledLayerNames = nullptr;

        VK_ASSERT(vkCreateDevice(_phy_device, &dci, nullptr, &_device));

        _graphics_queue = new VulkanQueue();
        _graphics_queue->_type = QUEUE_TYPE_GRAPHICS;
        _graphics_queue->_family_index = graphics_family;
        vkGetDeviceQueue(_device, _graphics_queue->_family_index, 0, &_graphics_queue->_queue);

        _compute_queue = new VulkanQueue();
        _compute_queue->_type = QUEUE_TYPE_COMPUTE;
        _compute_queue->_family_index = compute_family;
        vkGetDeviceQueue(_device, _compute_queue->_family_index, 0, &_compute_queue->_queue);

        _transfer_queue = new VulkanQueue();
        _transfer_queue->_type = QUEUE_TYPE_TRANSFER;
        _transfer_queue->_family_index = transfer_family;
        vkGetDeviceQueue(_device, _transfer_queue->_family_index, 0, &_transfer_queue->_queue);

        uint32_t num_sets                       = 65535;
        uint32_t num_sampled_images             = 32 * 65536;
        uint32_t num_storage_images             = 1  * 65536;
        uint32_t num_uniform_buffers            = 1  * 65536;
        uint32_t num_dynamic_uniform_buffers    = 4  * 65536;
        uint32_t num_storage_buffers            = 1  * 65536;
        uint32_t num_uniform_texel_buffers      = 8192;
        uint32_t num_storage_texel_buffers      = 8192;
        uint32_t num_samplers                   = 2  * 65536;

        VkDescriptorPoolSize poolSizes[8];

        poolSizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        poolSizes[0].descriptorCount = num_sampled_images;

        poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        poolSizes[1].descriptorCount = num_storage_images;

        poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[2].descriptorCount = num_uniform_buffers;

        poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        poolSizes[3].descriptorCount = num_dynamic_uniform_buffers;

        poolSizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[4].descriptorCount = num_storage_buffers;

        poolSizes[5].type = VK_DESCRIPTOR_TYPE_SAMPLER;
        poolSizes[5].descriptorCount = num_samplers;

        poolSizes[6].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        poolSizes[6].descriptorCount = num_uniform_texel_buffers;

        poolSizes[7].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        poolSizes[7].descriptorCount = num_storage_texel_buffers;

        VkDescriptorPoolCreateInfo dpci = {};
        dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        dpci.pNext = nullptr;
        dpci.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        dpci.maxSets = num_sets;
        dpci.poolSizeCount = 8;
        dpci.pPoolSizes = poolSizes;

        VK_ASSERT(vkCreateDescriptorPool(_device, &dpci, nullptr, &_descriptor_pool));
    }

    VulkanContext::~VulkanContext() {
        vkDeviceWaitIdle(_device);

        if (_graphics_queue) {
            delete _graphics_queue;
            _graphics_queue = nullptr;
        }

        if (_compute_queue) {
            delete _compute_queue;
            _compute_queue = nullptr;
        }

        if (_transfer_queue) {
            delete _transfer_queue;
            _transfer_queue = nullptr;
        }

        vkDestroyDescriptorPool(_device, _descriptor_pool, nullptr);
        vkDestroyDevice(_device, nullptr);

#if VULKAN_DEBUG
        if (_debug_messenger != VK_NULL_HANDLE) {
            vkDestroyDebugUtilsMessengerEXT(_instance, _debug_messenger, nullptr);
            _debug_messenger = VK_NULL_HANDLE;
        }
        if (_debug_report_callback != VK_NULL_HANDLE) {
            vkDestroyDebugReportCallbackEXT(_instance, _debug_report_callback, nullptr);
            _debug_report_callback = VK_NULL_HANDLE;
        }
#endif
        vkDestroyInstance(_instance, nullptr);
    }

    uint32_t VulkanContext::FindMemoryType(const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties) {
        for (uint32_t i = 0; i < _phy_device_memory_properties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (_phy_device_memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        return -1;
    }

    GfxQueue * VulkanContext::GetQueue(QueueType type) {
        if (type == QUEUE_TYPE_COMPUTE) {
            return _compute_queue;
        } else if (type == QUEUE_TYPE_TRANSFER) {
            return _transfer_queue;
        }
        else {
            return _graphics_queue;
        }
        return nullptr;
    }

    GfxSemaphore* VulkanContext::CreateSemaphore() {
        return new VulkanSemaphore(this);
    }

    void VulkanContext::DestroySemaphore(GfxSemaphore* semaphore) {
        BLAST_SAFE_DELETE(semaphore);
    }

    GfxFence* VulkanContext::CreateFence() {
        return new VulkanFence(this);
    }

    void VulkanContext::DestroyFence(GfxFence* fence) {
        BLAST_SAFE_DELETE(fence);
    }

    GfxBuffer* VulkanContext::CreateBuffer(const GfxBufferDesc& desc) {
        return new VulkanBuffer(this, desc);
    }

    void VulkanContext::DestroyBuffer(GfxBuffer* buffer) {
        BLAST_SAFE_DELETE(buffer);
    }

    GfxTexture* VulkanContext::CreateTexture(const GfxTextureDesc& desc) {
        return new VulkanTexture(this, desc);
    }

    void VulkanContext::DestroyTexture(GfxTexture* texture) {
        BLAST_SAFE_DELETE(texture);
    }

    GfxTextureView* VulkanContext::CreateTextureView(const GfxTextureViewDesc& desc) {
        return new VulkanTextureView(this, desc);
    }

    void VulkanContext::DestroyTextureView(GfxTextureView* view) {
        BLAST_SAFE_DELETE(view);
    }

    GfxSampler* VulkanContext::CreateSampler(const GfxSamplerDesc& desc) {
        return new VulkanSampler(this, desc);
    }

    void VulkanContext::DestroySampler(GfxSampler* sampler) {
        BLAST_SAFE_DELETE(sampler);
    }

    GfxSurface* VulkanContext::CreateSurface(const GfxSurfaceDesc& desc) {
        return new VulkanSurface(this, desc);
    }

    void VulkanContext::DestroySurface(GfxSurface* surface) {
        BLAST_SAFE_DELETE(surface);
    }

    GfxSwapchain* VulkanContext::CreateSwapchain(const GfxSwapchainDesc& desc) {
        return new VulkanSwapchain(this, desc);
    }

    void VulkanContext::DestroySwapchain(GfxSwapchain* swapchain) {
        BLAST_SAFE_DELETE(swapchain);
    }

    GfxCommandBufferPool* VulkanContext::CreateCommandBufferPool(const GfxCommandBufferPoolDesc& desc) {
        return new VulkanCommandBufferPool(this, desc);
    }

    void VulkanContext::DestroyCommandBufferPool(GfxCommandBufferPool* pool) {
        BLAST_SAFE_DELETE(pool);
    }

    GfxFramebuffer* VulkanContext::CreateFramebuffer(const GfxFramebufferDesc& desc) {
        return new VulkanFramebuffer(this, desc);
    }

    void VulkanContext::DestroyFramebuffer(GfxFramebuffer* framebuffer) {
        BLAST_SAFE_DELETE(framebuffer);
    }

    GfxShader* VulkanContext::CreateShader(const GfxShaderDesc& desc) {
        return new VulkanShader(this, desc);
    }

    void VulkanContext::DestroyShader(GfxShader* shader) {
        BLAST_SAFE_DELETE(shader);
    }

    GfxRootSignature* VulkanContext::CreateRootSignature(const GfxRootSignatureDesc& desc) {
        return new VulkanRootSignature(this, desc);
    }

    void VulkanContext::DestroyRootSignature(GfxRootSignature* root_signature) {
        BLAST_SAFE_DELETE(root_signature);
    }

    GfxGraphicsPipeline* VulkanContext::CreateGraphicsPipeline(const GfxGraphicsPipelineDesc& desc) {
        return new VulkanGraphicsPipeline(this, desc);
    }

    void VulkanContext::DestroyGraphicsPipeline(GfxGraphicsPipeline* pipeline) {
        BLAST_SAFE_DELETE(pipeline);
    }

    void VulkanContext::AcquireNextImage(GfxSwapchain* swapchain, GfxSemaphore* signal_semaphore, GfxFence* fence, uint32_t* image_index) {
        VkSemaphore vk_semaphore = VK_NULL_HANDLE;
        VkFence vk_fence = VK_NULL_HANDLE;

        VulkanSwapchain* internel_swapchain = static_cast<VulkanSwapchain*>(swapchain);

        if(signal_semaphore) {
            VulkanSemaphore* internel_semaphore = static_cast<VulkanSemaphore*>(signal_semaphore);
            vk_semaphore = internel_semaphore->GetHandle();
        }

        if(fence) {
            VulkanFence* internel_fence = static_cast<VulkanFence*>(fence);
            vk_fence = internel_fence->GetHandle();
        }

        VkResult result = vkAcquireNextImageKHR(_device, internel_swapchain->GetHandle(), UINT64_MAX, vk_semaphore, vk_fence, image_index);
        if (result != VK_SUCCESS) {
            // 当获取当前Image失败后，重置fence状态
            if (fence) {
                VulkanFence* internelFence = static_cast<VulkanFence*>(fence);
                internelFence->_submitted = false;
                vkResetFences(GetDevice(), 1, &internelFence->_fence);
            }
            *image_index = -1;
        }
    }

    VulkanSemaphore::VulkanSemaphore(VulkanContext* context)
        :GfxSemaphore(){
        _context = context;
        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VK_ASSERT(vkCreateSemaphore(_context->GetDevice(), &semaphore_info, nullptr, &_semaphore));
    }

    VulkanSemaphore::~VulkanSemaphore() {
        vkDestroySemaphore(_context->GetDevice(), _semaphore, nullptr);
    }

    VulkanFence::VulkanFence(VulkanContext* context)
        :GfxFence(){
        _context = context;
        _submitted = false;
        VkFenceCreateInfo fence_info = {};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // 指定创建Fence时的状态
        fence_info.flags = 0;
        VK_ASSERT(vkCreateFence(_context->GetDevice(), &fence_info, nullptr, &_fence));
    }

    VulkanFence::~VulkanFence() {
        vkDestroyFence(_context->GetDevice(), _fence, nullptr);
    }

    FenceStatus VulkanFence::GetFenceStatus() {
        if (_submitted) {
            VkResult result = vkGetFenceStatus(_context->GetDevice(), _fence);
            if(result == VK_SUCCESS) {
                _submitted = false;
                vkResetFences(_context->GetDevice(), 1, &_fence);
                return FENCE_STATUS_COMPLETE;
            } else {
                return FENCE_STATUS_INCOMPLETE;
            }
        }
        return FENCE_STATUS_NOTSUBMITTED;
    }

    void VulkanFence::WaitForComplete() {
        if (!_submitted) {
            return;
        }
        vkWaitForFences(_context->GetDevice(), 1, &_fence, VK_TRUE, UINT64_MAX);
        vkResetFences(_context->GetDevice(), 1, &_fence);
        _submitted = false;
    }

    VulkanQueue::VulkanQueue() {
    }

    VulkanQueue::~VulkanQueue() {
    }

    void VulkanQueue::Submit(const GfxSubmitInfo& info) {
        std::vector<VkPipelineStageFlags> wait_stages;
        std::vector<VkSemaphore> wait_semaphores;
        std::vector<VkSemaphore> signal_semaphores;
        std::vector<VkCommandBuffer> cmds;

        for(int i = 0; i < info.num_wait_semaphores; i++) {
            wait_stages.push_back(VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
            VulkanSemaphore* semaphore = static_cast<VulkanSemaphore*>(info.wait_semaphores[i]);
            wait_semaphores.push_back(semaphore->GetHandle());
        }

        for(int i = 0; i < info.num_signal_semaphores; i++) {
            VulkanSemaphore* semaphore = static_cast<VulkanSemaphore*>(info.signal_semaphores[i]);
            signal_semaphores.push_back(semaphore->GetHandle());
        }

        for (int i = 0; i < info.num_cmd_bufs; ++i) {
            VulkanCommandBuffer* cmd = static_cast<VulkanCommandBuffer*>(info.cmd_bufs[i]);
            cmds.push_back(cmd->GetHandle());
        }

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = wait_semaphores.size();
        submit_info.pWaitSemaphores = wait_semaphores.data();
        submit_info.pWaitDstStageMask = wait_stages.data();
        submit_info.signalSemaphoreCount = signal_semaphores.size();
        submit_info.pSignalSemaphores = signal_semaphores.data();
        submit_info.commandBufferCount = cmds.size();
        submit_info.pCommandBuffers = cmds.data();

        if (info.signal_fence) {
            VulkanFence* fence = static_cast<VulkanFence*>(info.signal_fence);
            fence->_submitted = true;
            VK_ASSERT(vkQueueSubmit(_queue, 1, &submit_info, fence->GetHandle()));
        } else {
            VK_ASSERT(vkQueueSubmit(_queue, 1, &submit_info, VK_NULL_HANDLE));
        }
    }

    int VulkanQueue::Present(const GfxPresentInfo& info) {
        std::vector<VkSemaphore> wait_semaphores;
        for(int i = 0; i < info.num_wait_semaphores; i++) {
            VulkanSemaphore* semaphore = static_cast<VulkanSemaphore*>(info.wait_semaphores[i]);
            wait_semaphores.push_back(semaphore->GetHandle());
        }

        VulkanSwapchain* swapchain = static_cast<VulkanSwapchain*>(info.swapchain);

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = wait_semaphores.size();
        present_info.pWaitSemaphores = wait_semaphores.data();
        VkSwapchainKHR swapChains[] = { swapchain->GetHandle() };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapChains;
        present_info.pImageIndices = &info.index;

        VkResult result = vkQueuePresentKHR(_queue, &present_info);
        if (result != VK_SUCCESS) {
            return -1;
        }
        return 0;
    }

    void VulkanQueue::WaitIdle() {
        vkQueueWaitIdle(_queue);
    }
}
