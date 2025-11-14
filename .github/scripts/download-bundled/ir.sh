#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/ir
rm -rf "$tmp_dir"

revision=5a81104e650ebd7ac24eb63d4dff67db723a5278

git clone --depth 1 --revision="$revision" https://github.com/dstogov/ir.git "$tmp_dir"

rm -rf ext/opcache/jit/ir
cp -R "$tmp_dir" ext/opcache/jit/ir

cd ext/opcache/jit/ir

# remove unneeded files
rm -r .git
rm -r .github
rm -r bench
rm -r examples
rm -r tests
rm -r tools
rm README.md
rm TODO
rm ir.g
rm ir_cpuinfo.c
rm ir_emit_c.c
rm ir_emit_llvm.c
rm ir_load.c
rm ir_load_llvm.c
rm ir_main.c
rm ir_mem2ssa.c

# add extra files
git restore README
