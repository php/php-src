--TEST--
Enum preloading
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/gh8133.inc
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php

var_dump(Foo::CASES);
var_dump(Qux::CASES);

?>
--EXPECT--
array(2) {
  [0]=>
  enum(Foo::Bar)
  [1]=>
  enum(Foo::Baz)
}
array(2) {
  [0]=>
  enum(Foo::Bar)
  [1]=>
  enum(Foo::Baz)
}
