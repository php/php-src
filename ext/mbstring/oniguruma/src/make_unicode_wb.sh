#!/bin/sh

NAME=unicode_wb_data

python3 make_unicode_wb_data.py > ${NAME}.c

exit 0
