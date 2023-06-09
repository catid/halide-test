#include "generators_tools.h"

#include "schedules/sparse_affine_grads.schedule.h"

class sparse_affine_grads_generator : public Halide::Generator<sparse_affine_grads_generator>
{
public:
    // Expecting normalized values between 0..1
    // Image gradients in x, y
    Input<Buffer<float>> gradients{"gradients", 3};
    // Selected sampling locations
    Input<Buffer<uint16_t>> local_max{"local_max", 3};
    // Should have same dimensions as local_max
    Output<Buffer<float>> output{"output", 3};

    static const int N = 6; // Number of parameters

    void generate() 
    {
        // HACK: This min() works around a Halide bug
        Expr ix = min(local_max(x, y, 0), gradients.width() - 1);
        Expr iy = min(local_max(x, y, 1), gradients.height() - 1);

        Expr inv_w = 1.f / cast<float>( gradients.width() );
        Expr inv_h = 1.f / cast<float>( gradients.height() );

        Expr x_norm = ix * inv_w;
        Expr y_norm = iy * inv_h;

        output(x, y, c) = mux(c, {
            gradients(ix, iy, 0) * (1.f - x_norm  - y_norm),
            gradients(ix, iy, 0) * x_norm,
            gradients(ix, iy, 0) * y_norm,
            gradients(ix, iy, 1) * (1.f - x_norm  - y_norm),
            gradients(ix, iy, 1) * x_norm,
            gradients(ix, iy, 1) * y_norm
        });
    }

    void schedule()
    {
        gradients.dim(0).set_estimate(128, expected_width);
        gradients.dim(1).set_estimate(128, expected_height);
        gradients.dim(2).set_estimate(2, 2);
        local_max.dim(0).set_estimate(16, expected_width / SPARSE_MAXIMA_N);
        local_max.dim(1).set_estimate(16, expected_height / SPARSE_MAXIMA_N * SPARSE_MAXIMA_SAMPLES);
        local_max.dim(2).set_estimate(2, 2);
        output.set_estimate(x, 1, expected_width / SPARSE_MAXIMA_N).set_estimate(y, 1, expected_height / SPARSE_MAXIMA_N * SPARSE_MAXIMA_SAMPLES).set_estimate(c, N, N);

#if HALIDE_VERSION_MAJOR >= 15
        bool auto_schedule = using_autoscheduler();
#endif
        if (auto_schedule) {
            return;
        }

        if (get_target().has_gpu_feature())
        {
            // FIXME
            output.compute_root().gpu_tile(x, y, xo, yo, xi, yi, 8, 8);
        }
        else
        {
            apply_schedule_sparse_affine_grads(get_pipeline(), get_target());
        }
    }
};

HALIDE_REGISTER_GENERATOR(sparse_affine_grads_generator, sparse_affine_grads)
