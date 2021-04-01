--TEST--
Preloading: Loadable class checking (3)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_loadable_classes_3.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
class Foo {
}
var_dump(new Test);
?>
--EXPECT--
object(Test)#1 (0) {
  ["prop":protected]=>
  uninitialized(Foo)
}
