--TEST--
ReflectionClass::isSubclassOf() with wrong-case class name fails with wrong case
--FILE--
<?php
class Base {}
class Child extends Base {}

$rc = new ReflectionClass(Child::class);

var_dump($rc->isSubclassOf("base"));
var_dump($rc->isSubclassOf("BASE"));
var_dump($rc->isSubclassOf("Base"));
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "base" does not exist in %s:%d
Stack trace:
#0 %s(7): ReflectionClass->isSubclassOf('base')
#1 {main}
  thrown in %s on line %d
