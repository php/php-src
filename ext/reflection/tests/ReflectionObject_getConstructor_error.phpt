--TEST--
ReflectionObject::getConstructor() - invalid params
--FILE--
<?php
class C {}
$rc = new ReflectionObject(new C);
var_dump($rc->getConstructor(null));
var_dump($rc->getConstructor('X'));
var_dump($rc->getConstructor(true));
var_dump($rc->getConstructor(array(1,2,3)));
?>
--EXPECTF--
Warning: ReflectionClass::getConstructor() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getConstructor() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getConstructor() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getConstructor() expects exactly 0 parameters, 1 given in %s on line %d
NULL
