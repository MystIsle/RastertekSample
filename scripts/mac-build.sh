#!/usr/bin/env bash
# macOS에서 Windows exe 크로스컴파일
# 필요: brew install cmake mingw-w64
set -euo pipefail
cd "$(dirname "$0")/.."

BUILD_DIR=build-mac
CONFIG="${1:-Debug}"

cmake -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-x86_64.cmake \
    -DCMAKE_BUILD_TYPE="$CONFIG" \
    -G Ninja 2>/dev/null || \
cmake -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-x86_64.cmake \
    -DCMAKE_BUILD_TYPE="$CONFIG"

cmake --build "$BUILD_DIR"
echo
echo "빌드 완료: $BUILD_DIR/RastertekSample.exe"
