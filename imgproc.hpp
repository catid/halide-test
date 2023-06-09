#pragma once

#include <Halide.h>

#include "constants.h"

void ImgProcTest(
    Halide::Runtime::Buffer<float>& gradxy,
    Halide::Runtime::Buffer<uint16_t>& local_max,
    Halide::Runtime::Buffer<float>& sparse_affine_images);
