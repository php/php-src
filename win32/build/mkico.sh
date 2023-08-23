#!/usr/bin/env bash

for I in 16 32 64 128 256;
do
    convert -size ${I}x${I} php.svg php-ico-${I}.png;
done

convert -adjoin php-ico-* php.ico

rm -f php-ico-*.png
