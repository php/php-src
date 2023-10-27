#!/usr/bin/env bash

magick convert -size 512x512 php.svg php.bmp

for I in 16 24 32 48 64 128 256;
do
    magick convert -scale ${I}x${I} php.bmp php-ico-${I}.png;
done

magick convert -adjoin php-ico-* php.ico

rm -f php-ico-*.png php.bmp
