--TEST--
Object keys in arrays
--FILE--
<?php

$array = [];
$obj1 = new stdClass;
$obj2 = new stdClass;

$array[$obj1] = 1;
$array[$obj2] = 2;
var_dump($array[$obj1]);
var_dump($array[$obj2]);

unset($array[$obj2]);
var_dump($array[$obj2]);

var_dump(isset($array[$obj1]));
var_dump(isset($array[$obj2]));

$array[$obj1]++;
$array[$obj1] += 1;
var_dump($array[$obj1]);

?>
--EXPECTF--
int(1)
int(2)

Warning: Undefined array key stdClass#2 in %s on line %d
NULL
bool(true)
bool(false)
int(3)
