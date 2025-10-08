--TEST--
GH-20073 (Assertion failure in WeakMap offset operations on reference)
--FILE--
<?php
$obj = new stdClass;
$map = new WeakMap;
$integer = 1;
$map[$obj] = 0;
$map[$obj] =& $integer;
$integer++;
var_dump($map[$obj], $map->offsetGet($obj));
?>
--EXPECT--
int(2)
int(2)
