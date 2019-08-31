--TEST--
Preloading class using trait with undefined class constant access
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_undef_const_2.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(trait_exists('T'));
var_dump(class_exists('Foo'));
?>
--EXPECTF--
Warning: Can't preload class Foo with unresolved initializer for constant C in %spreload_undef_const_2.inc on line 8
bool(true)
bool(false)
