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

    Halide::Runtime::Buffer<uint8_t> input(w, h);
    Halide::Runtime::Buffer<uint8_t> output(w, h);

    // Random x/y image gradients:

    for (int i = 0; i < input.height(); ++i) {
        for (int j = 0; j < input.width(); ++j) {
            input(j, i) = static_cast<uint8_t>( dis(gen) * 255 );
        }
    }

    const int trials = 1000;
    for (int i = 0; i < trials; ++i) {
        uint64_t t0 = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
        ImgProcTest(input, output);
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
