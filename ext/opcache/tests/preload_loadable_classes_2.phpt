--TEST--
Preloading: Loadable class checking (2)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_loadable_classes_2.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
const UNDEF = 1;
class Foo {
    const UNDEF = 2;
}
var_dump(class_exists("Test"));
var_dump(Test::X);
var_dump(Test::Y);
?>
--EXPECT--
bool(true)
int(1)
int(2)
