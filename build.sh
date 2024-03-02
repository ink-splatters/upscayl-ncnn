#!/usr/bin/env bash

set -e
set -o pipefail

CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME:-Darwin}

CMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS:--fuse-ld=lld}
OpenMP_C_FLAGS=${OpenMP_C_FLAGS:--Xclang -fopenmp}
OpenMP_CXX_FLAGS=${OpenMP_CXX_FLAGS:--Xclang -fopenmp}
USE_STATIC_MOLTENVK=${USE_STATIC_MOLTENVK:-ON}
CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES:-arm64}
CMAKE_CROSSCOMPILING=${CMAKE_CROSSCOMPILING:-ON}
CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR:-arm64}


if [ "$1" = "--debug" ]; then
    CMAKE_C_FLAGS="-fno-limit-debug-info -g ${CMAKE_C_FLAGS}"
    CMAKE_CXX_FLAGS="-fno-limit-debug-info -g ${CMAKE_CXX_FLAGS}"
else
    CMAKE_C_FLAGS=${CMAKE_C_FLAGS:--O3}
    CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS:--O3}
fi

mkdir -p build
cmake \
    -DCMAKE_SYSTEM_NAME="$CMAKE_SYSTEM_NAME" \
    -DCMAKE_C_FLAGS="$CMAKE_C_FLAGS" \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_EXE_LINKER_FLAGS="$CMAKE_EXE_LINKER_FLAGS" \
    -DOpenMP_C_FLAGS="$OpenMP_C_FLAGS" \
    -DOpenMP_CXX_FLAGS="$OpenMP_CXX_FLAGS" \
    -DUSE_STATIC_MOLTENVK="$USE_STATIC_MOLTENVK" \
    -DCMAKE_OSX_ARCHITECTURES="$CMAKE_OSX_ARCHITECTURES" \
    -DCMAKE_CROSSCOMPILING="$CMAKE_CROSSCOMPILING" \
    -DCMAKE_SYSTEM_PROCESSOR="$CMAKE_SYSTEM_PROCESSOR" \
    -G Ninja \
    -S src \
    -B build

cmake --build build
