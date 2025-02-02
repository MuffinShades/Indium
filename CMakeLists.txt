#create le project
cmake_minimum_required(VERSION 3.15)
project(indium VERSION 0.0.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES
    pch/*.h
    porcupinetree/*.cpp
    porcupinetree/*.c
    porcupinetree/*.hpp
    porcupinetree/*.h
)

add_executable(indium ${SOURCES})

target_include_directories(indium PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/porcupinetree
    ${CMAKE_CURRENT_SOURCE_DIR}/pch
)