--TEST--
Bug #78918: Class alias during preloading causes assertion failure
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_class_alias.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(class_exists('A'));
var_dump(class_exists('B'));
?>
--EXPECT--
bool(true)
bool(true)
