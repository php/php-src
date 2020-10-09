--TEST--
ReflectionObject::isSubclassOf() - invalid params
--FILE--
<?php
class A {}
$ro = new ReflectionObject(new A);

var_dump($ro->isSubclassOf('X'));

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "X" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->isSubclassOf('X')
#1 {main}
  thrown in %s on line %d
