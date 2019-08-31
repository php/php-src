--TEST--
Nested function definition
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_nested_function.inc
opcache.interned_strings_buffer=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
test();
test2();
?>
===DONE===
--EXPECT--
===DONE===
