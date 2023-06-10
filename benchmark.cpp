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

    Halide::Runtime::Buffer<float> gradients(w, h, 2);
    Halide::Runtime::Buffer<float> output(w, h, 4);
    //Halide::Runtime::Buffer<float> gradients = Halide::Runtime::Buffer<float>::make_interleaved(w, h, 2);
    //Halide::Runtime::Buffer<float> output = Halide::Runtime::Buffer<float>::make_interleaved(w, h, 4);

    // Random x/y image gradients:

    for (int i = 0; i < gradients.height(); ++i) {
        for (int j = 0; j < gradients.width(); ++j) {
            gradients(j, i, 0) = dis(gen);
            gradients(j, i, 1) = dis(gen);
        }
    }

    const int trials = 10000;
    for (int i = 0; i < trials; ++i) {
        uint64_t t0 = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        ImgProcTest(gradients, output);
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
