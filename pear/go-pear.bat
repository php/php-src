@ECHO OFF
set PHP_BIN=cli\php.exe
%PHP_BIN% -d output_buffering=0 PEAR\go-pear.php %1
pause
