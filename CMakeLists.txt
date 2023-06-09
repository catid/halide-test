cmake_minimum_required(VERSION 3.15)
project(halide_test CXX)


################################################################################
# Dependencies

# Must specify cmake -DHALIDE_VERSION=14.0.0
if(DEFINED HALIDE_VERSION)
    set(HALIDE_VERSION ${HALIDE_VERSION} CACHE STRING "Halide version")
elseif(DEFINED ENV{HALIDE_VERSION})
    set(HALIDE_VERSION $ENV{HALIDE_VERSION} CACHE STRING "Halide version")
else()
    set(HALIDE_VERSION "14.0.0" CACHE STRING "Halide version")
endif()

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" HALIDE_VERSION_MAJOR ${HALIDE_VERSION})
string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\..*" "\\1" HALIDE_VERSION_MINOR ${HALIDE_VERSION})
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1" HALIDE_VERSION_PATCH ${HALIDE_VERSION})

add_definitions(-DHALIDE_VERSION_MAJOR=${HALIDE_VERSION_MAJOR})
add_definitions(-DHALIDE_VERSION_MINOR=${HALIDE_VERSION_MINOR})
add_definitions(-DHALIDE_VERSION_PATCH=${HALIDE_VERSION_PATCH})

message("Halide version specified: ${HALIDE_VERSION}: ${HALIDE_VERSION_MAJOR}.${HALIDE_VERSION_MINOR}.${HALIDE_VERSION_PATCH}")

# Expect to find halide under ./distro/
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/distro/Halide-${HALIDE_VERSION}-x86-64-linux/lib/cmake/Halide")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/distro/Halide-${HALIDE_VERSION}-x86-64-linux/lib/cmake/HalideHelpers")

find_package(Halide REQUIRED)


################################################################################
# Configuration

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# C++ language version
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug/Release Modes
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g ${AIK_DEBUG_OPT} -march=native -fstack-protector")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

# In debug mode, add -DDEBUG
add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")

# Static library: -fPIC
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")


################################################################################
# Library: imgproc

add_library(imgproc SHARED
    imgproc.cpp
    imgproc.hpp
)

target_link_libraries(imgproc
    PUBLIC
        Halide::Halide
        Threads::Threads
)


################################################################################
# Generators

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "64-bit build")
    set(HALIDE_BITS 64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "32-bit build")
    set(HALIDE_BITS 32)
else()
    message(FATAL_ERROR "Unknown architecture")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(STATUS "Configuring on Linux")
    set(HALIDE_OS "linux")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    message(STATUS "Configuring on macOS")
    set(HALIDE_OS "osx")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    message(STATUS "Configuring on Windows")
    set(HALIDE_OS "windows")
else()
    message(FATAL_ERROR "Unknown OS ${CMAKE_SYSTEM_NAME}")
endif()

if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "arm")
    message(STATUS "ARM architecture detected")
    set(HALIDE_TARGET_ARCH "arm-${HALIDE_BITS}-${HALIDE_OS}")
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "amd64|AMD64|x86_64|X86_64")
    message(STATUS "Intel architecture detected")
    set(HALIDE_TARGET_ARCH "x86-64-${HALIDE_OS}-avx-avx2-f16c-fma-sse41")
else()
    message(FATAL_ERROR "Unknown architecture is ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif()

function(generate_target)

    # Pass through arguments to usual function from Halide
    add_halide_library(${ARGV}
        # Configure it for the target architecture
        TARGETS
            ${HALIDE_TARGET_ARCH}
    )

    message("Configuring generator: ${ARGV0}")

    set(GENERATED_HEADERS ${GENERATED_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.h PARENT_SCOPE)

    # "whole-archive" is not supported on OSX.
    if (APPLE)
        set(WHOLE_ARCHIVE_FLAG "-all_load")
        set(NO_WHOLE_ARCHIVE_FLAG "-noall_load")
    else()
        set(WHOLE_ARCHIVE_FLAG "--whole-archive")
        set(NO_WHOLE_ARCHIVE_FLAG "--no-whole-archive")
    endif()

    # This causes the generated library to get sucked up into the imgproc library
    # and the symbol persists even if it's not used by anything
    target_link_libraries(imgproc
        PUBLIC
            "-Wl,${WHOLE_ARCHIVE_FLAG}"
            ${ARGV0}
            "-Wl,${NO_WHOLE_ARCHIVE_FLAG}"
    )
endfunction()

add_executable(generators
    generators.cpp
    generators_tools.cpp
)
target_link_libraries(generators PRIVATE Halide::Generator)

generate_target(sparse_affine_grads FROM generators
    GENERATOR sparse_affine_grads
    #AUTOSCHEDULER Halide::Adams2019
    #SCHEDULE ON
)


################################################################################
# Apps

# App: benchmark

add_executable(benchmark
    benchmark.cpp
)
target_link_libraries(benchmark
    imgproc
)
install(TARGETS benchmark DESTINATION bin)
