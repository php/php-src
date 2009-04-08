--TEST--
ReflectionObject::getname() - invalid params
--FILE--
<?php
class C { }
$myInstance = new C;
$r2 = new ReflectionObject($myInstance);

$r3 = new ReflectionObject($r2);

var_dump($r3->getName(null));
var_dump($r3->getName('x','y'));
var_dump($r3->getName(0));
?>
--EXPECTF--
Warning: ReflectionClass::getName() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getName() expects exactly 0 parameters, 2 given in %s on line %d
NULL

Warning: ReflectionClass::getName() expects exactly 0 parameters, 1 given in %s on line %d
NULL
