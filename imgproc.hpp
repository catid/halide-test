#pragma once

#include <Halide.h>

#include "constants.h"

void ImgProcTest(
    Halide::Runtime::Buffer<float>& gradients,
    Halide::Runtime::Buffer<float>& output);
