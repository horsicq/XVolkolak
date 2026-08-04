#!/usr/bin/env bash
set -euo pipefail

project_root="$(cd "$(dirname "$0")/../.." && pwd)"
build_dir="${TMPDIR:-/tmp}/xvolkolak_macos_build"

rm -rf "$build_dir"
cmake -S "$project_root" -B "$build_dir" -DCMAKE_BUILD_TYPE=Release "${@}"
cmake --build "$build_dir" --config Release
cpack --config "$build_dir/CPackConfig.cmake" -G ZIP
