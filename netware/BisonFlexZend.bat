@echo off

bison --output=zend_ini_parser.c -v -d -p ini_ zend_ini_parser.y
flex -B -i -Sflex.skl -Pini_ -ozend_ini_scanner.c zend_ini_scanner.l
REM flex -i -Pini_ -ozend_ini_scanner.c zend_ini_scanner.l
bison --output=zend_language_parser.c -v -d -p zend zend_language_parser.y
flex -B -i -Sflex.skl -Pzend -ozend_language_scanner.c zend_language_scanner.l
REM flex -i -Pzend -ozend_language_scanner.c zend_language_scanner.l
