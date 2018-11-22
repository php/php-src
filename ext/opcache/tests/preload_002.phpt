--TEST--
Preloading prototypes
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
