--TEST--
Crash in observer API calling cases() on a compile-only preloaded enum
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_enum_compile_only.inc
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_output=0
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php

var_dump(MyEnum::cases());

?>
--EXPECT--
array(2) {
  [0]=>
  enum(MyEnum::Foo)
  [1]=>
  enum(MyEnum::Bar)
}
