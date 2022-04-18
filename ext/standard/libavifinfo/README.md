# AVIF-info

There is no compact, reliable way to determine the size of an AVIF image. A
standalone C snippet called
[libavifinfo](https://aomedia.googlesource.com/libavifinfo) was created to
partially parse an AVIF payload and to extract the width, height, bit depth and
channel count without depending on the full libavif library.

`avifinfo.h`, `avifinfo.c`, `LICENSE` and `PATENTS` were copied verbatim from: \
https://aomedia.googlesource.com/libavifinfo/+/96f34d945ac7dac229feddfa94dbae66e202b838 \
They can easily be kept up-to-date the same way.
