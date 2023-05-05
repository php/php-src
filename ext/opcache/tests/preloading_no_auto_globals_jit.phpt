--TEST--
Preloading with auto_globals_jit=0
--INI--
auto_globals_jit=0
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/preloading_no_auto_globals_jit.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
$test = new Test;
var_dump($test->count_global_server());
?>
--EXPECTF--
int(%d)
