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
Warning: Wrong parameter count for ReflectionClass::getConstants() in %s on line 8

Warning: Wrong parameter count for ReflectionClass::getConstants() in %s on line 9

Warning: Wrong parameter count for ReflectionClass::getConstants() in %s on line 10

Warning: Wrong parameter count for ReflectionClass::getConstants() in %s on line 11
