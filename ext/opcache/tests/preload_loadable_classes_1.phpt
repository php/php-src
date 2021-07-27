--TEST--
Preloading: Loadable class checking (1)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_loadable_classes_1.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
var_dump(class_exists('Test'));
var_dump(class_exists('Bar'));
var_dump(class_exists('Foo'));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
