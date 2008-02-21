--TEST--
Test wrong number of arguments for rad2deg()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float rad2deg(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(rad2deg($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(rad2deg());

?>
--EXPECTF--
Too many arguments

Warning: rad2deg() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Too few arguments

Warning: rad2deg() expects exactly 1 parameter, 0 given in %s on line %d
NULL
