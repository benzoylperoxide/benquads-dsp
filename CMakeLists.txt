cmake_minimum_required(VERSION 3.22)
project(dsp-core VERSION 0.1)

add_library(dsp-core INTERFACE)
target_include_directories(dsp-core INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(dsp-core INTERFACE cxx_std_17)

option(DSP_CORE_BUILD_TESTS "Build tests" OFF)
if(DSP_CORE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
