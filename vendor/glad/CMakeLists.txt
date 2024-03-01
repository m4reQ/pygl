cmake_minimum_required(VERSION 3.18)
project(glad)

option(GLAD_BUILD_SHARED "If set to ON glad will be built as shared library. (default: OFF)" OFF)

if(${GLAD_BUILD_SHARED})
    add_library(glad SHARED "src/gl.c")
else()
    add_library(glad STATIC "src/gl.c")
endif()

target_include_directories(glad PUBLIC "include")
