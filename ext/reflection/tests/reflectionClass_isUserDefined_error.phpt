--TEST--
ReflectionClass::isUserDefined() - invalid params.
--FILE--
<?php
$r1 = new ReflectionClass("stdClass");
var_dump($r1->isUserDefined('X'));
var_dump($r1->isUserDefined('X', true));
?>
--EXPECTF--
Warning: Wrong parameter count for ReflectionClass::isUserDefined() in %s on line 3
NULL

Warning: Wrong parameter count for ReflectionClass::isUserDefined() in %s on line 4
NULL
