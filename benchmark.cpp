#include <iostream>
#include <vector>
#include <chrono>
#include <cstdint>
#include <algorithm>
using namespace std;

#include "imgproc.hpp"


//------------------------------------------------------------------------
// Benchmark

void Benchmark()
{
    std::vector<int> micros;

    const int seed = 1234;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(0.0, 1.0);

    int w = 1280;
    int h = 1024;

    Halide::Runtime::Buffer<float> gradxy(w, h, 2);
    Halide::Runtime::Buffer<uint16_t> local_max(w/SPARSE_MAXIMA_N, h/SPARSE_MAXIMA_N*SPARSE_MAXIMA_SAMPLES, 2);
    Halide::Runtime::Buffer<float> sparse_affine_images(w/SPARSE_MAXIMA_N, h/SPARSE_MAXIMA_N*SPARSE_MAXIMA_SAMPLES, 6);

    // Random x/y image gradients:

    for (int i = 0; i < gradxy.height(); ++i) {
        for (int j = 0; j < gradxy.width(); ++j) {
            gradxy(j, i, 0) = dis(gen);
            gradxy(j, i, 1) = dis(gen);
        }
    }

    // Simulate local maxima:

    for (int i = 0; i < local_max.height(); ++i) {
        for (int j = 0; j < local_max.width(); ++j) {
            int rx = static_cast<int>( dis(gen) * SPARSE_MAXIMA_N );
            int ry = static_cast<int>( dis(gen) * SPARSE_MAXIMA_N );
            int x = SPARSE_MAXIMA_N * j + rx;
            int y = SPARSE_MAXIMA_N * i + ry;

            gradxy(j, i, 0) = static_cast<uint16_t>(x);
            gradxy(j, i, 1) = static_cast<uint16_t>(y);
        }
    }

    const int trials = 100;
    for (int i = 0; i < 100; ++i) {
        uint64_t t0 = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        ImgProcTest(gradxy, local_max, sparse_affine_images);
        uint64_t t1 = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        micros.push_back(t1 - t0);
    }

    std::sort(micros.begin(), micros.end());

    int median_usec = micros[micros.size() / 2];

    cout << "Median time: " << median_usec << " usec" << endl;
}


//------------------------------------------------------------------------
// Entrypoint

int main()
{
    Benchmark();
    return 0;
}
