--TEST--
ReflectionClass::getName() - invalid params
--FILE--
<?php

$r1 = new ReflectionClass("stdClass");

var_dump($r1->getName('X'));
var_dump($r1->getName('X', true));
?>
--EXPECTF--
Warning: ReflectionClass::getName() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionClass::getName() expects exactly 0 parameters, 2 given in %s on line %d
NULL
