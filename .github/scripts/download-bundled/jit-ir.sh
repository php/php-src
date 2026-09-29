#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/jit-ir
rm -rf "$tmp_dir"

revision=00bec1ca490b8bc51f636a42040c5e9f64c1a91b

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

# patch customized files
git apply -v ../../../../.github/scripts/download-bundled/jit-ir.patch
