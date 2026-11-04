#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/jit-ir
rm -rf "$tmp_dir"

revision=30a40633b0b8f3e8e66da0a25941c4199d69d38a

git clone --depth 1 --revision="$revision" https://github.com/dstogov/ir.git "$tmp_dir"

rm -rf ext/opcache/jit/ir
cp -R "$tmp_dir" ext/opcache/jit/ir

cd ext/opcache/jit/ir

# remove unneeded files
rm -rf .git
rm -r .github
rm -r bench
rm -r examples
rm -r tests
rm -r tools
rm -r fuzz
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
