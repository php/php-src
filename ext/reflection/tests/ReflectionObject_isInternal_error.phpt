--TEST--
ReflectionObject::isInternal() - invalid params
--FILE--
<?php

$r1 = new ReflectionObject(new stdClass);
var_dump($r1->isInternal('X'));
var_dump($r1->isInternal('X', true));
?>
--EXPECTF--
Warning: ReflectionClass::isInternal() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::isInternal() expects exactly 0 parameters, 2 given in %s on line %d
NULL
