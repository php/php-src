--TEST--
ReflectionObject::isSubclassOf() - invalid params
--FILE--
<?php
class A {}
$ro = new ReflectionObject(new A);

var_dump($ro->isSubclassOf());
var_dump($ro->isSubclassOf('A',5));
var_dump($ro->isSubclassOf('X'));

?>
--EXPECTF--
Warning: ReflectionClass::isSubclassOf() expects exactly 1 parameter, 0 given in %s on line 5
NULL

Warning: ReflectionClass::isSubclassOf() expects exactly 1 parameter, 2 given in %s on line 6
NULL

Fatal error: Uncaught exception 'ReflectionException' with message 'Class X does not exist' in %s:7
Stack trace:
#0 %s(7): ReflectionClass->isSubclassOf('X')
#1 {main}
  thrown in %s on line 7
