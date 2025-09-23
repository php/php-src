#!/bin/sh

NAME=unicode_egcb_data

python3 make_unicode_egcb_data.py > ${NAME}.c

exit 0
