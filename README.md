# halide-test
Test v14/v15 performance regression

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

cd ..
```

## Running

Run the benchmark:

```bash
mkdir build
cd build
cmake ..
make -j
./benchmark
```

Edit the CMakeLists.txt to specify `HALIDE_VERSION=v15.0.1` and run it again.
