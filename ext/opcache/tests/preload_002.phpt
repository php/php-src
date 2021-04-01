--TEST--
Preloading prototypes
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump((new ReflectionMethod('x', 'foo'))->getPrototype()->class);
var_dump((new ReflectionMethod('x', 'bar'))->getPrototype()->class);
?>
OK
--EXPECT--
string(1) "b"
string(1) "a"
OK
