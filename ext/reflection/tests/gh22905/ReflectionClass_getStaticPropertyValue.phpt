--TEST--
GH-22905: null bytes in ReflectionClass::getStaticPropertyValue() error messages
--FILE--
<?php

class Demo {}
$r = new ReflectionClass(Demo::class);
$r->getStaticPropertyValue("foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Property Demo::$foo%0bar does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->getStaticPropertyValue('foo\x00bar')
#1 {main}
  thrown in %s on line %d
