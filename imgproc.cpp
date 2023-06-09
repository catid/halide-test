#include "imgproc.hpp"

#include "sparse_affine_grads.h"

void ImgProcTest(
    Halide::Runtime::Buffer<float>& gradxy,
    Halide::Runtime::Buffer<uint16_t>& local_max,
    Halide::Runtime::Buffer<float>& sparse_affine_images)
{
    sparse_affine_grads(gradxy, local_max, sparse_affine_images);
}
