#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

commit=b2bd4254b379b9d7dc9a3dda060a7e27009ccdff # 10.46 release

git clone --recurse-submodules --revision=$commit https://github.com/PCRE2Project/pcre2.git /tmp/php-src-bundled/pcre2

rm -rf ext/pcre/pcre2lib
cp -R /tmp/php-src-bundled/pcre2/src ext/pcre/pcre2lib
cp -R /tmp/php-src-bundled/pcre2/deps/sljit/sljit_src ext/pcre/pcre2lib/sljit

cd ext/pcre/pcre2lib

# remove unneeded files
rm pcre2.h.in
rm pcre2_dftables.c
rm pcre2_fuzzsupport.c
rm pcre2_jit_test.c
rm pcre2demo.c
rm pcre2grep.c
rm pcre2posix.c
rm pcre2posix.h
rm pcre2posix_test.c
rm pcre2test.c

# move renamed files
mv config.h.generic config.h
mv pcre2.h.generic pcre2.h
mv pcre2_chartables.c.dist pcre2_chartables.c

# add extra files
git restore config.h # based on config.h.generic but with many changes

# patch customized files
git apply -v ../../../.github/scripts/download-bundled/pcre2-adjust-sljit-directory.patch
