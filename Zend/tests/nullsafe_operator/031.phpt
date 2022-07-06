--TEST--
Nullsafe operator on referenced value
--FILE--
<?php

$val = null;
$ref =& $val;
var_dump($ref?->foo);

$val = new stdClass;
var_dump($ref?->foo);

?>
--EXPECTF--
NULL

Warning: Undefined property: stdClass::$foo in %s on line %d
NULL
