#include "imgproc.hpp"

#include "similarity_grads.h"

void ImgProcTest(
    Halide::Runtime::Buffer<float>& gradients,
    Halide::Runtime::Buffer<float>& output)
{
    similarity_grads(gradients, output);
}
