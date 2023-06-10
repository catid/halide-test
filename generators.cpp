#include "generators_tools.h"

#define ENABLE_AUTO_SCHEDULE

#if !defined(ENABLE_AUTO_SCHEDULE)
#include "schedules/similarity_grads.schedule.h"
#endif

class similarity_grads_generator : public Halide::Generator<similarity_grads_generator>
{
public:
    // Expecting normalized values between 0..1
    Input<Buffer<float>> gradients{"gradients", 3};
    Output<Buffer<float>> output{"output", 3};

    static const int N = 6; // Number of parameters

    void generate() 
    {
        output(x, y, c) = mux(c, {
            gradients(x, y, 0) * x + gradients(x, y, 1) * y,
            gradients(x, y, 0) * (-y) + gradients(x, y, 1) * x,
            gradients(x, y, 0),
            gradients(x, y, 1)
        });
    }

    void schedule()
    {
        gradients.dim(0).set_estimate(128, expected_width);
        gradients.dim(1).set_estimate(128, expected_height);
        gradients.dim(2).set_estimate(2, 2);
        output.set_estimate(x, 128, expected_width).set_estimate(y, 128, expected_height).set_estimate(c, N, N);

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
#if !defined(ENABLE_AUTO_SCHEDULE)
            apply_schedule_sparse_affine_grads(get_pipeline(), get_target());
#endif
        }
    }
};

HALIDE_REGISTER_GENERATOR(similarity_grads_generator, similarity_grads)
