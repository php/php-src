#!/bin/sh
# afaik the PHP project doesn't allow git submodules, so we do this fetcher script instead.
cd "$(dirname "$0")"
rm -rf upstream_blake3
# fancy way of fetching only the files we want
git clone --branch '1.5.0' -n --depth=1 --filter=tree:0 'https://github.com/BLAKE3-team/BLAKE3.git' 'upstream_blake3'
cd upstream_blake3
git sparse-checkout set --no-cone LICENSE c/blake3.c  c/blake3.h  c/blake3_avx2.c  c/blake3_avx2_x86-64_unix.S \
 c/blake3_avx2_x86-64_windows_gnu.S  c/blake3_avx2_x86-64_windows_msvc.asm  c/blake3_avx512.c \
 c/blake3_avx512_x86-64_unix.S  c/blake3_avx512_x86-64_windows_gnu.S  c/blake3_avx512_x86-64_windows_msvc.asm \
 c/blake3_dispatch.c  c/blake3_impl.h  c/blake3_neon.c  c/blake3_portable.c  c/blake3_sse2.c \
 c/blake3_sse2_x86-64_unix.S  c/blake3_sse2_x86-64_windows_gnu.S  c/blake3_sse2_x86-64_windows_msvc.asm \
 c/blake3_sse41.c  c/blake3_sse41_x86-64_unix.S  c/blake3_sse41_x86-64_windows_gnu.S \
 c/blake3_sse41_x86-64_windows_msvc.asm
git checkout
rm -rf .git
cd ..
git apply patches.diff
