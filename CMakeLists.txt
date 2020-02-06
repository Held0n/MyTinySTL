cmake_minimum_required(VERSION 3.15)
project(TinySTL)

# version
set(TinySTL_VERSION 1.0.0)
message("TinySTL version ${TinySTL_VERSION}")

# build type
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-but-set-variable -Wno-array-bounds")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0.0")
        message(FATAL_ERROR "required GCC 5.0 or later")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
endif()

message(STATUS "the cmake_cxx_flags: ${CMAKE_CXX_FLAGS}")

add_subdirectory(${PROJECT_SOURCE_DIR}/TinySTL)
