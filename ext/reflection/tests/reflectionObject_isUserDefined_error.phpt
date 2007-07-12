--TEST--
ReflectionObject::isUserDefined() - invalid params
--FILE--
<?php
$r1 = new ReflectionObject(new stdClass);

var_dump($r1->isUserDefined('X'));
var_dump($r1->isUserDefined('X', true));
?>
--EXPECTF--
Warning: Wrong parameter count for ReflectionClass::isUserDefined() in %s on line 4
NULL

Warning: Wrong parameter count for ReflectionClass::isUserDefined() in %s on line 5
NULL
