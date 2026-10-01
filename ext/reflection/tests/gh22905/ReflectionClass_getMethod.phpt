--TEST--
GH-22905: null bytes in ReflectionClass::getMethod() error messages
--FILE--
<?php

class Demo {}
$r = new ReflectionClass(Demo::class);
$r->getMethod("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Method Demo::foo%0bar() does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->getMethod('foo\x00bar')
#1 {main}
  thrown in %s on line %d
