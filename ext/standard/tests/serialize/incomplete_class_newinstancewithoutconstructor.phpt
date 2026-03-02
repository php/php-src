--TEST--
ReflectionClass::newInstanceWithoutConstructor cannot instantiate __PHP_Incomplete_Class
--FILE--
<?php
$rc = new ReflectionClass("__PHP_Incomplete_Class");
$rc->newInstanceWithoutConstructor();
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class __PHP_Incomplete_Class is an internal class marked as final that cannot be instantiated without invoking its constructor in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->newInstanceWithoutConstructor()
#1 {main}
  thrown in %s on line %d
