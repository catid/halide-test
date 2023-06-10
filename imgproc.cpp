#include "imgproc.hpp"

#include "halide15_bug.h"

void ImgProcTest(
    Halide::Runtime::Buffer<uint8_t>& input,
    Halide::Runtime::Buffer<uint8_t>& output
)
{
    halide15_bug(input, output);
}
