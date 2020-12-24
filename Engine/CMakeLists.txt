if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
add_library(BlastEngine SHARED)
target_include_directories(BlastEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_sources(BlastEngine PUBLIC
        Source/Gfx/GfxDevice.cpp
        Source/Gfx/GfxBuffer.cpp
        Source/Gfx/GfxTexture.cpp
        Source/Gfx/GfxRenderTarget.cpp
        Source/Gfx/GfxSwapchain.cpp
        Source/Gfx/Vulkan/VulkanDefine.cpp
        Source/Gfx/Vulkan/VulkanDevice.cpp
        Source/Gfx/Vulkan/VulkanBuffer.cpp
        Source/Gfx/Vulkan/VulkanTexture.cpp
        Source/Gfx/Vulkan/VulkanRenderTarget.cpp
        Source/Gfx/Vulkan/VulkanSwapchain.cpp)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_compile_definitions(BlastEngine PUBLIC VULKAN_DEBUG)
endif()

add_library(volk STATIC External/volk/volk.c External/volk/volk.h)
target_include_directories(volk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/volk)
target_include_directories(volk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/khronos/vulkan-headers/include)
if (WIN32)
    target_compile_definitions(volk PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()
target_link_libraries(BlastEngine PUBLIC volk)

if (WIN32)
    install(TARGETS BlastEngine
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib)
endif()