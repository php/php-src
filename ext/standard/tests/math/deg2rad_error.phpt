--TEST--
Test wrong number of arguments for deg2rad()
--INI--
precision = 14
--FILE--
<?php
/*
 * proto float deg2rad(float number)
 * Function is implemented in ext/standard/math.c
*/

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(deg2rad($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(deg2rad());

?>
--EXPECTF--
Too many arguments

Warning: deg2rad() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Too few arguments

Warning: deg2rad() expects exactly 1 parameter, 0 given in %s on line %d
NULL
