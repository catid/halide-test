# halide-test
Demonstrate v14/v15 performance regression with .compute_at().

Note this same simple codebase runs optimized on Jetson Nano (ARM64), Ubuntu Linux on Intel, and Windows with Visual Studio. Without writing any platform-specific code.


## Setup

Requires an Ubuntu Linux server running on a modern Intel CPU.

```bash
git clone https://github.com/catid/halide-test
cd halide-test

sudo apt install cmake clang llvm
```

Set up Halide:

```bash
mkdir distros
cd distros

wget https://github.com/halide/Halide/releases/download/v14.0.0/Halide-14.0.0-x86-64-linux-6b9ed2afd1d6d0badf04986602c943e287d44e46.tar.gz
tar vzxf Halide-14.0.0-x86-64-linux-6b9ed2afd1d6d0badf04986602c943e287d44e46.tar.gz

wget https://github.com/halide/Halide/releases/download/v15.0.1/Halide-15.0.1-x86-64-linux-4c63f1befa1063184c5982b11b6a2cc17d4e5815.tar.gz
tar vzxf Halide-15.0.1-x86-64-linux-4c63f1befa1063184c5982b11b6a2cc17d4e5815.tar.gz

# Or the release for your platform from: https://github.com/halide/Halide/releases/

cd ..
```

## Running

Run the benchmark with v14:

```bash
mkdir build14
cd build14
cmake -DHALIDE_VERSION=14.0.0 ..
make -j
./benchmark
```

Note the speed.  Should be the same each time within 10 microseconds.

Run the benchmark with v15:

```bash
mkdir build15
cd build15
cmake -DHALIDE_VERSION=15.0.1 ..
make -j
./benchmark
```

## Results

Output from V14:

```
(base) ➜  build14 git:(bug15) ✗ ./benchmark
Median time: 138 usec
```

Output from V15:

```
(base) ➜  build15 git:(bug15) ✗ ./benchmark
Median time: 407 usec
```

This performance difference increases to 5x slower if I add a RDom inside the pipeline.
Adding more intermediate functions with .compute_at() also increases the slowdown.
