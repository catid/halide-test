#pragma once

#include <Halide.h>

#include "constants.h"

void ImgProcTest(
    Halide::Runtime::Buffer<uint8_t>& input,
    Halide::Runtime::Buffer<uint8_t>& output
);
