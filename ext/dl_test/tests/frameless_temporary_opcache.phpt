--TEST--
dl() does not compile calls to a temporary module's function as frameless
--SKIPIF--
<?php include __DIR__ . "/skip.inc"; ?>
--EXTENSIONS--
opcache
--INI--
enable_dl=1
opcache.enable_cli=1
opcache.opt_debug_level=0x10000
--FILE--
<?php
dl(PHP_OS_FAMILY === 'Windows' ? 'php_dl_test.dll' : 'dl_test.so');
include __DIR__ . '/frameless_temporary.inc';
?>
--EXPECTF--
%A0001 INIT_FCALL 1 %d string("dl_test_frameless")
%Aint(7)
dl_test_frameless(): Argument #1 ($value) must be of type int, string given
