@echo off

bison --output=ext\standard\parsedate.c -v -d -p phplib ext\standard\parsedate.y
