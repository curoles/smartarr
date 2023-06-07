cmake_minimum_required(VERSION 3.22)

# Top level CMake script for building SMARTARR.
# Author: Igor Lesik 2023

project(SMARTARR
    VERSION 0.1.0
    DESCRIPTION "C library of array functions"
    LANGUAGES C #ASM C #no-CXX
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    # require at least gcc 13.1
    set(GCC_MIN_VER "13.1")
    if (CMAKE_C_COMPILER_VERSION VERSION_LESS GCC_MIN_VER)
        message(FATAL_ERROR
            "GCC version must be at least ${GCC_MIN_VER}! found ${CMAKE_C_COMPILER_VERSION}")
    endif()
endif()

set(C_WARN_FLAGS  "-Wall -Wextra -Werror")
set(C_OPT_FLAGS   "-O3 -march=native -ftree-vectorize")
set(C_EXTRA_FLAGS "-fopenmp")
set(CMAKE_C_FLAGS "${C_WARN_FLAGS} ${C_OPT_FLAGS} ${C_EXTRA_FLAGS}")
#set(CMAKE_CXX_FLAGS "...")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-z,stack-size=1000000")


include_directories(./src/include)

# Install inside the build directory.
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

enable_testing()

#add_subdirectory(test)

message(STATUS "Host   CPU  : ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "Target CPU  : ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler    : ${CMAKE_C_COMPILER_ID}")
message(STATUS "Compiler ver: ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "CFLAGS      : ${CMAKE_C_FLAGS}")
