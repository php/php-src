#!/bin/sh
set -e
cd "$(dirname "$0")/../../.."

commit=8d40e0306aacca3596f64a338b5033d2050fc20e

git clone --revision=$commit https://github.com/zherczeg/sljit.git /tmp/php-src-bundled/sljit
