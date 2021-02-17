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
        Source/Gfx/GfxRenderTarget.cpp
        Source/Gfx/GfxSwapchain.cpp
        Source/Gfx/GfxCommandBuffer.cpp
        Source/Gfx/GfxPipeline.cpp
        Source/Gfx/GfxShader.cpp
        Source/Gfx/Vulkan/VulkanDefine.cpp
        Source/Gfx/Vulkan/VulkanContext.cpp
        Source/Gfx/Vulkan/VulkanBuffer.cpp
        Source/Gfx/Vulkan/VulkanTexture.cpp
        Source/Gfx/Vulkan/VulkanRenderTarget.cpp
        Source/Gfx/Vulkan/VulkanSwapchain.cpp
        Source/Gfx/Vulkan/VulkanCommandBuffer.cpp
        Source/Gfx/Vulkan/VulkanPipeline.cpp
        Source/Gfx/Vulkan/VulkanShader.cpp)

if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_compile_definitions(Blast PUBLIC VULKAN_DEBUG)
endif()

add_library(volk STATIC External/volk/volk.c External/volk/volk.h)
target_include_directories(volk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/volk)
target_include_directories(volk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/Vulkan-headers/include)
if (WIN32)
    target_compile_definitions(volk PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()
target_link_libraries(Blast PUBLIC volk)

#set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
#set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

#if (WIN32)
#    install(TARGETS Blast
#            RUNTIME DESTINATION bin
#            ARCHIVE DESTINATION lib
#            LIBRARY DESTINATION lib)
#endif()