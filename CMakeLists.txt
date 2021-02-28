cmake_minimum_required(VERSION 3.13)
project(Blast)
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
add_library(Blast SHARED)
target_include_directories(Blast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_sources(Blast PUBLIC
        Source/Gfx/GfxContext.cpp
        Source/Gfx/GfxBuffer.cpp
        Source/Gfx/GfxTexture.cpp
        Source/Gfx/GfxRenderPass.cpp
        Source/Gfx/GfxSwapchain.cpp
        Source/Gfx/GfxCommandBuffer.cpp
        Source/Gfx/GfxPipeline.cpp
        Source/Gfx/GfxShader.cpp
        Source/Gfx/Vulkan/VulkanDefine.cpp
        Source/Gfx/Vulkan/VulkanContext.cpp
        Source/Gfx/Vulkan/VulkanBuffer.cpp
        Source/Gfx/Vulkan/VulkanTexture.cpp
        Source/Gfx/Vulkan/VulkanRenderPass.cpp
        Source/Gfx/Vulkan/VulkanSwapchain.cpp
        Source/Gfx/Vulkan/VulkanCommandBuffer.cpp
        Source/Gfx/Vulkan/VulkanPipeline.cpp
        Source/Gfx/Vulkan/VulkanShader.cpp
        Source/Utility/VulkanShaderCompiler.cpp)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_compile_definitions(Blast PUBLIC VULKAN_DEBUG)
endif()

# volk
add_library(volk STATIC External/volk/volk.c External/volk/volk.h)
target_include_directories(volk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/volk)
target_include_directories(volk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/Vulkan-headers/include)
if (WIN32)
    target_compile_definitions(volk PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()
target_link_libraries(Blast PUBLIC volk)

# glslang
set(BUILD_EXTERNAL OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/External/glslang EXCLUDE_FROM_ALL)
target_link_libraries(Blast PRIVATE glslang)
target_link_libraries(Blast PRIVATE HLSL)
target_link_libraries(Blast PRIVATE OGLCompiler)
target_link_libraries(Blast PRIVATE OSDependent)
target_link_libraries(Blast PRIVATE SPIRV)
target_link_libraries(Blast PRIVATE SPVRemapper)

set(SPIRV_CROSS_ENABLE_TESTS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/External/SPIRV-Cross EXCLUDE_FROM_ALL)
target_link_libraries(Blast PRIVATE spirv-cross-c)
target_link_libraries(Blast PRIVATE spirv-cross-core)
target_link_libraries(Blast PRIVATE spirv-cross-cpp)
target_link_libraries(Blast PRIVATE spirv-cross-glsl)
target_link_libraries(Blast PRIVATE spirv-cross-hlsl)
target_link_libraries(Blast PRIVATE spirv-cross-msl)
target_link_libraries(Blast PRIVATE spirv-cross-reflect)
target_link_libraries(Blast PRIVATE spirv-cross-util)

#if (WIN32)
#    install(TARGETS Blast
#            RUNTIME DESTINATION bin
#            ARCHIVE DESTINATION lib
#            LIBRARY DESTINATION lib)
#endif()