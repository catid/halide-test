#ifndef sparse_affine_grads_SCHEDULE_H
#define sparse_affine_grads_SCHEDULE_H

// MACHINE GENERATED -- DO NOT EDIT
// This schedule was automatically generated by Adams2019
// for target=x86-64-linux-avx-avx2-f16c-fma-sse41  // NOLINT
// with autoscheduler_params=autoscheduler=Adams2019

#include "Halide.h"


inline void apply_schedule_sparse_affine_grads(
    ::Halide::Pipeline pipeline,
    ::Halide::Target target
) {
    using ::Halide::Func;
    using ::Halide::MemoryType;
    using ::Halide::RVar;
    using ::Halide::TailStrategy;
    using ::Halide::Var;
    Func output = pipeline.get_func(2);
    Var c(output.get_schedule().dims()[2].var);
    Var ci("ci");
    Var x(output.get_schedule().dims()[0].var);
    Var xi("xi");
    Var xii("xii");
    Var y(output.get_schedule().dims()[1].var);
    Var yi("yi");
    output
        .split(x, x, xi, 32, TailStrategy::ShiftInwards)
        .split(y, y, yi, 11, TailStrategy::ShiftInwards)
        .split(c, c, ci, 3, TailStrategy::ShiftInwards)
        .split(xi, xi, xii, 16, TailStrategy::ShiftInwards)
        .vectorize(xii)
        .compute_root()
        .reorder({xii, xi, yi, ci, x, y, c})
        .fuse(y, c, y)
        .fuse(x, y, x)
        .parallel(x);

}

#endif  // sparse_affine_grads_SCHEDULE_H
