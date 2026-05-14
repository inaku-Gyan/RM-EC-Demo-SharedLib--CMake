cmake_minimum_required(VERSION 3.24)

project(ecx
    VERSION 0.1.0
    DESCRIPTION "Header-only shared library for embedded RoboMaster projects"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(ecx INTERFACE)
add_library(ecx::ecx ALIAS ecx)

target_include_directories(ecx INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_compile_features(ecx INTERFACE cxx_std_20)

option(ECX_BUILD_TESTS     "Build host-side tests" ${PROJECT_IS_TOP_LEVEL})
option(ECX_BUILD_ARM_CHECK "Compile algo+proto with arm-none-eabi-gcc as a smoke check" OFF)

if(ECX_BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(ECX_BUILD_ARM_CHECK)
    add_library(ecx_arm_check STATIC cmake/arm_compile_check.cpp)
    target_link_libraries(ecx_arm_check PRIVATE ecx::ecx)
endif()
