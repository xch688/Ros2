#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

PKG_DIR="${PKG_DIR:-chap02/workspace/demo_cpp_pkg}"
TARGET_NAME="${TARGET_NAME:-cpp_node}"

PKG_PATH="${ROOT_DIR}/${PKG_DIR}"
BUILD_DIR="${PKG_PATH}/build"

cmake -S "${PKG_PATH}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug
if [[ -n "${TARGET_NAME}" ]]; then
  cmake --build "${BUILD_DIR}" --target "${TARGET_NAME}" -j "$(nproc)"
else
  cmake --build "${BUILD_DIR}" -j "$(nproc)"
fi
