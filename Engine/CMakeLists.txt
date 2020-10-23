if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
add_library(BlastEngine SHARED)
target_include_directories(BlastEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(BlastEngine PUBLIC test.cpp)

if (WIN32)
    install(TARGETS BlastEngine
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib)
endif()