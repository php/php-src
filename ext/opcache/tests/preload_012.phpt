--TEST--
No autoloading during constant resolution
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_const_autoload.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
--EXPECTF--
Warning: Can't preload class Test with unresolved initializer for constant C in %s on line %d
