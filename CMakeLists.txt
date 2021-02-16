cmake_minimum_required(VERSION 3.13)
project(Blast)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(Core)
if(DEFINED EnableBlastExamples)
    add_subdirectory(Examples)
endif()
