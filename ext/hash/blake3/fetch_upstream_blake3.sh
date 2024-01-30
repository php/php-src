#!/bin/sh
# afaik the PHP project doesn't allow git submodules, so we do this fetcher script instead.
cd "$(dirname "$0")"
rm -rf upstream_blake3
# fancy way of just fetching the "c" folder (the only thing we want)
git clone --branch '1.5.0' -n --depth=1 --filter=tree:0 'https://github.com/BLAKE3-team/BLAKE3.git' 'upstream_blake3'
cd upstream_blake3
git sparse-checkout set --no-cone c
git checkout
rm -rf .git
cd c
# some stuff we don't need
rm -rf blake3_c_rust_bindings test.py example.c main.c Makefile.testing CMakeLists.txt blake3-config.cmake.in README.md .gitignore libblake3.pc.in
