--TEST--
Bug #79114 (Eval class during preload causes class to be only half available)
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
var_dump(class_exists(Foo::class));
var_dump(class_exists(Bar::class));
new Bar();
var_dump(class_parents('Bar'));
new Foo();
f3();
?>
--EXPECT--
bool(true)
bool(true)
array(1) {
  ["Foo"]=>
  string(3) "Foo"
}
