--TEST--
ReflectionObject::isInternal() - invalid params
--FILE--
<?php

$r1 = new ReflectionObject(new stdClass);
var_dump($r1->isInternal('X'));
var_dump($r1->isInternal('X', true));
?>
--EXPECTF--
Warning: Wrong parameter count for ReflectionClass::isInternal() in %s on line 4
NULL

Warning: Wrong parameter count for ReflectionClass::isInternal() in %s on line 5
NULL
