--TEST--
Preloading dynamic def in method/function
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_dynamic_def_removal.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
dynamic();
dynamic2();
?>
--EXPECT--
dynamic
dynamic2
