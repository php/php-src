#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

commit=5a81104e650ebd7ac24eb63d4dff67db723a5278

rm -rf ext/opcache/jit/ir
git clone --revision=$commit https://github.com/dstogov/ir.git ext/opcache/jit/ir

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
