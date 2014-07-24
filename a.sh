#!/bin/bash

set -e

./buildconf --force
./configure --enable-mbstring --enable-debug
make

./sapi/cli/php -d zend.multibyte=On -d mbstring.internal_encoding="EUC_JP"  a.php
