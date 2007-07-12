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
Warning: Wrong parameter count for ReflectionClass::getConstants() in %s on line 7

Warning: Wrong parameter count for ReflectionClass::getConstants() in %s on line 8

