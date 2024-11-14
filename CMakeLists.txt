cmake_minimum_required(VERSION 3.24)
project(pygl)

# Change option behavior for compatibility with cglm
cmake_policy(SET CMP0077 NEW)

# Find Python development libraries
find_package(Python REQUIRED COMPONENTS Development.Module)

# add GLAD as dependency
add_subdirectory(vendor/glad)

# add CGLM as dependency
add_subdirectory(vendor/cglm EXCLUDE_FROM_ALL)

# find source files
file(GLOB_RECURSE SOURCES "src_c/*.c")

# setup library
add_library(pygl MODULE ${SOURCES})

target_link_libraries(pygl PRIVATE ${Python_LIBRARIES} glad cglm_headers)
target_include_directories(pygl PRIVATE ${Python_INCLUDE_DIRS})
target_compile_definitions(pygl PRIVATE "PY_SSIZE_T_CLEAN")

target_precompile_headers(
    pygl PRIVATE
    <Python.h>
    <structmember.h>
    <glad/gl.h>
    <stdbool.h>)

set_target_properties(
    pygl PROPERTIES
    C_STANDARD 17
    OUTPUT_NAME "pygl$<$<CONFIG:Debug>:_d>" # if debug mode is used output filename with _d suffix
    PREFIX ""
    SUFFIX ".pyd"
    COMPILE_WARNING_AS_ERROR ON)
