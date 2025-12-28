--TEST--
GH-18567 (Preloading with internal class alias triggers assertion failure)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/preload_gh18567.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
abstract class Test implements MyStringable {
}
$rc = new ReflectionClass(Test::class);
var_dump($rc->getInterfaces());
?>
--EXPECT--
array(1) {
  ["Stringable"]=>
  object(ReflectionClass)#2 (1) {
    ["name"]=>
    string(10) "Stringable"
  }
}
