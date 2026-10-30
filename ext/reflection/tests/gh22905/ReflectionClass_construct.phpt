--TEST--
GH-22905: null bytes in ReflectionClass::__construct() error messages
--FILE--
<?php

new ReflectionClass("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "foo%0bar" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->__construct('foo\x00bar')
#1 {main}
  thrown in %s on line %d
