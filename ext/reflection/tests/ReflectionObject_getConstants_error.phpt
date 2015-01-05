--TEST--
ReflectionObject::getConstants() - invalid params
--FILE--
<?php
class X {
}

$rc = new ReflectionObject(new X);

$rc->getConstants('X');
$rc->getConstants(true);

?>
--EXPECTF--
Warning: ReflectionClass::getConstants() expects exactly 0 parameters, 1 given in %s on line %d

Warning: ReflectionClass::getConstants() expects exactly 0 parameters, 1 given in %s on line %d
