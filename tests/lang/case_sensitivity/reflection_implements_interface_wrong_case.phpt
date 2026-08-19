--TEST--
ReflectionClass::implementsInterface() with wrong-case interface name fails with wrong case
--FILE--
<?php
interface MyInterface {}
class MyClass implements MyInterface {}

$rc = new ReflectionClass(MyClass::class);

var_dump($rc->implementsInterface("myinterface"));
var_dump($rc->implementsInterface("MYINTERFACE"));
var_dump($rc->implementsInterface("MyInterface"));
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Interface "myinterface" does not exist in %s:%d
Stack trace:
#0 %s(7): ReflectionClass->implementsInterface('myinterface')
#1 {main}
  thrown in %s on line %d
