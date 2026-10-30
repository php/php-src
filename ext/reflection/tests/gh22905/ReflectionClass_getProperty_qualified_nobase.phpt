--TEST--
GH-22905: null bytes in ReflectionClass::getProperty() (fully qualified, non-base class) error messages
--FILE--
<?php

class Base {}
class Demo {}
$r = new ReflectionClass(Demo::class);
$r->getProperty("Base::foo\0bar");

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Fully qualified property name Base::$foo%0bar does not specify a base class of Demo in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->getProperty('Base::foo\x00bar')
#1 {main}
  thrown in %s on line %d
