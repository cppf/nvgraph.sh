#!/usr/bin/env bash
# Download & install nvGraph
git clone --recurse-submodules https://github.com/rapidsai/nvgraph
export CUDA_ROOT="/usr/local/cuda"
cd nvgraph && ./build.sh

# Build and Install the C/C++ CUDA components
# mkdir -p cpp/build
# cd cpp/build
# cmake .. -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX
# make -j && make install

# C++ stand alone tests
cd cpp/build
gtests/NVGRAPH_TEST
