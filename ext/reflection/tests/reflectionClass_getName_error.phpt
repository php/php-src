--TEST--
ReflectionClass::getName() - invalid params
--FILE--
<?php

$r1 = new ReflectionClass("stdClass");

var_dump($r1->getName('X'));
var_dump($r1->getName('X', true));
?> 
--EXPECTF--
Warning: Wrong parameter count for ReflectionClass::getName() in %s on line 5
NULL

Warning: Wrong parameter count for ReflectionClass::getName() in %s on line 6
NULL
 
