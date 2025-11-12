#!/bin/sh
set -ex
cd "$(dirname "$0")/../../.."

revision=refs/tags/pcre2-10.44

git clone --depth 1 --recurse-submodules --revision="$revision" https://github.com/PCRE2Project/pcre2.git /tmp/php-src-bundled/pcre2

rm -rf ext/pcre/pcre2lib
cp -R /tmp/php-src-bundled/pcre2/src ext/pcre/pcre2lib

cd ext/pcre/pcre2lib

# remove unneeded files
rm config.h.generic
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
mv pcre2.h.generic pcre2.h
mv pcre2_chartables.c.dist pcre2_chartables.c

# add extra files
git restore config.h # based on config.h.generic but with many changes
