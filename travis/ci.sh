#!/usr/bin/env sh
git clone https://github.com/php/php-src
git checkout $PHP
cd php-src/sapi
rm -rf phpdbg
git clone https://github.com/krakjoe/phpdbg.git
cd ../
./buildconf --force
./configure --disable-all --enable-phpdbg --enable-maintainer-zts
make
