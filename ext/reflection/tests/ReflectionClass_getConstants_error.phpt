--TEST--
ReflectionClass::getConstants()
--FILE--
<?php
class X {
}

$rc = new reflectionClass('X');

//Test invalid arguments
$rc->getConstants('X');
$rc->getConstants(true);
$rc->getConstants(null);
$rc->getConstants('A', 'B');

?>
--EXPECTF--
Warning: ReflectionClass::getConstants() expects exactly 0 parameters, 1 given in %s on line %d

Warning: ReflectionClass::getConstants() expects exactly 0 parameters, 1 given in %s on line %d

Warning: ReflectionClass::getConstants() expects exactly 0 parameters, 1 given in %s on line %d

Warning: ReflectionClass::getConstants() expects exactly 0 parameters, 2 given in %s on line %d
