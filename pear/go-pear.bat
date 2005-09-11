@ECHO OFF
set PHP_BIN=php.exe
%PHP_BIN% -d output_buffering=0 go-pear.phar
pause
