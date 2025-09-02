#!/usr/bin/env bash

convert php.svg -size 512x512  php.bmp

for I in 16 24 32 48 64 128 256;
do
    convert php.bmp -scale ${I}x${I} php-ico-${I}.png;
done

convert -adjoin php-ico-* php.ico

rm -f php-ico-*.png php.bmp
