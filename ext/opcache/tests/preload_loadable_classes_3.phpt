--TEST--
Preloading: Loadable class checking (3)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_loadable_classes_3.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
Unreachable
--EXPECTF--
Fatal error: Failed to load class Foo used by typed property Test::$prop during preloading in Unknown on line 0
