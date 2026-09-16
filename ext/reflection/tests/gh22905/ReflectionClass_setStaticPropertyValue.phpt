--TEST--
GH-22905: null bytes in ReflectionClass::setStaticPropertyValue() error messages
--FILE--
<?php

class Demo {}
$r = new ReflectionClass(Demo::class);
$r->setStaticPropertyValue("foo\0bar", 123);

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Property Demo::$foo%0bar does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->setStaticPropertyValue('foo\x00bar', 123)
#1 {main}
  thrown in %s on line %d
