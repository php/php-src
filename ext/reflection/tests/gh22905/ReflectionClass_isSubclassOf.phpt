--TEST--
GH-22905: null bytes in ReflectionClass::isSubclassOf() error messages
--FILE--
<?php

class Demo {}
$r = new ReflectionClass(Demo::class);
$r->isSubclassOf("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "foo%0bar" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->isSubclassOf('foo\x00bar')
#1 {main}
  thrown in %s on line %d
