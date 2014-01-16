#!/usr/bin/env sh
git clone https://github.com/php/php-src
cd php-src/sapi
git clone https://github.com/krakjoe/phpdbg.git
cd ../
./buildconf --force
./configure --disable-all --enable-phpdbg --enable-maintainer-zts
make
make test-phpdbg
