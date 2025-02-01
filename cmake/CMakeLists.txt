#create le project
cmake_minimum_required(VERSION 3.15)
project(moop VERSION 0.0.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES
    ../porcupinetree/*.cpp
    ../porcupinetree/*.c
    ../porcupinetree/*.hpp
    ../porcupinetree/*.h
    ../pch/*.h
)

add_executable(indium ${SOURCES})