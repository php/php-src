#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/blake3
rm -rf "$tmp_dir"

revision=refs/tags/1.8.3

git clone --depth 1 --revision="$revision" https://github.com/BLAKE3-team/BLAKE3.git "$tmp_dir"

rm -rf ext/hash/blake3/upstream_blake3
mkdir ext/hash/blake3/upstream_blake3
cp -R "$tmp_dir"/c ext/hash/blake3/upstream_blake3
cp "$tmp_dir"/LICENSE_CC0 ext/hash/blake3/upstream_blake3

cd ext/hash/blake3/upstream_blake3

# remove unneeded files
rm -R c/blake3_c_rust_bindings
rm -R c/cmake
rm -R c/dependencies
rm c/.gitignore
rm c/blake3_tbb.cpp
rm c/blake3-config.cmake.in
rm c/CMakeLists.txt
rm c/CMakePresets.json
rm c/example.c
rm c/example_tbb.c
rm c/libblake3.pc.in
rm c/main.c
rm c/Makefile.testing
rm c/README.md
rm c/test.py

# patch customized files
git apply -v ../../../../.github/scripts/download-bundled/blake3.h.patch
