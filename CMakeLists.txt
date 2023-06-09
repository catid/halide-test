cmake_minimum_required(VERSION 3.15)
project(halide_test CXX)


################################################################################
# Dependencies

# Select version of Halide
#set(HALIDE_VERSION "14.0.0")
set(HALIDE_VERSION "15.0.1")
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
# Library: image_processing

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

set(HALIDE_TARGET_ARCH "x86-64-linux-avx-avx2-f16c-fma-sse41")

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

    # This causes the generated library to get sucked up into the image_processing.so
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
    AUTOSCHEDULER Halide::Adams2019
#    SCHEDULE ON
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
