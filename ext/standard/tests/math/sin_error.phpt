--TEST--
Test wrong number of arguments for sin()
--FILE--
<?php
/*
 * proto float sin(float number)
 * Function is implemented in ext/standard/math.c
*/

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(sin($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(sin());

?>
--EXPECTF--
Too many arguments

Warning: sin() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Too few arguments

Warning: sin() expects exactly 1 parameter, 0 given in %s on line %d
NULL
