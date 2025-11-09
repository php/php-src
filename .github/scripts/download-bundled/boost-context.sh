#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

tmp_dir=/tmp/php-src-download-bundled/boost-context
rm -rf "$tmp_dir"

revision=refs/tags/boost-1.86.0

git clone --depth 1 --revision="$revision" https://github.com/boostorg/context.git "$tmp_dir"

rm -rf Zend/asm
cp -R "$tmp_dir"/src/asm Zend/asm

cd Zend/asm

# remove unneeded files
rm jump_arm_aapcs_pe_armasm.asm
rm jump_i386_ms_pe_clang_gas.S
rm jump_i386_ms_pe_gas.asm
rm jump_i386_x86_64_sysv_macho_gas.S
rm jump_ppc32_ppc64_sysv_macho_gas.S
rm jump_x86_64_ms_pe_clang_gas.S
rm make_arm_aapcs_pe_armasm.asm
rm make_i386_ms_pe_clang_gas.S
rm make_i386_ms_pe_gas.asm
rm make_i386_x86_64_sysv_macho_gas.S
rm make_ppc32_ppc64_sysv_macho_gas.S
rm make_x86_64_ms_pe_clang_gas.S
rm ontop_*.S
rm ontop_*.asm
rm tail_ontop_ppc32_sysv.cpp

# move renamed files
# GH-13896 introduced these 2 files named as .S but since https://github.com/boostorg/context/pull/265 they are named as .asm
mv jump_x86_64_ms_pe_gas.asm jump_x86_64_ms_pe_gas.S
mv make_x86_64_ms_pe_gas.asm make_x86_64_ms_pe_gas.S

# add extra files
git restore LICENSE
git restore save_xmm_x86_64_ms_masm.asm # added in GH-18352, not an upstream boost.context file
