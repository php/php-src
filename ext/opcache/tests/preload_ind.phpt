--TEST--
Various tests that need an opcache_compile_file() indirected preload file
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_ind.inc
--SKIPIF--
<?php
require_once('skipif.inc');
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
OK
--EXPECTF--
Warning: Can't preload class C with unresolved property types in %s on line %d
OK
