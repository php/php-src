#!/bin/sh

# Run this script after updating the bundled sqlite library

cc -o sqlite/tool/lemon sqlite/tool/lemon.c
./sqlite/tool/lemon sqlite/src/parse.y
cat sqlite/src/parse.h sqlite/src/vdbe.c | awk -f sqlite/mkopcodeh.awk > sqlite/src/opcodes.h
sort -n +2 sqlite/src/opcodes.h | awk -f sqlite/mkopcodec.awk > sqlite/src/opcodes.c
cc -o sqlite/tool/mkkeywordhash sqlite/tool/mkkeywordhash.c
./sqlite/tool/mkkeywordhash > sqlite/src/keywordhash.h
