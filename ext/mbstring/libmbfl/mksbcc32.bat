@echo off
cd %2
%1 -f Makefile.bcc32 %3
cd ..
exit 
