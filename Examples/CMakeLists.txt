add_executable(BlastEditor main.cpp)

target_link_libraries(BlastEditor PRIVATE BlastEngine)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/External/glfw EXCLUDE_FROM_ALL)
target_link_libraries(BlastEditor PRIVATE glfw)