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
Warning: Wrong parameter count for ReflectionClass::getName() in %s on line 8
NULL

Warning: Wrong parameter count for ReflectionClass::getName() in %s on line 9
NULL

Warning: Wrong parameter count for ReflectionClass::getName() in %s on line 10
NULL
