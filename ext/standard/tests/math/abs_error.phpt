--TEST--
Test abs() function :  error conditions - incorrect number of args
--FILE--
<?php
/* Prototype  : number abs  ( mixed $number  )
 * Description: Returns the absolute value of number.
 * Source code: ext/standard/math.c
 */

/*
 * Pass incorrect number of arguments to abs() to test behaviour
 */
 
echo "*** Testing abs() : error conditions ***\n";

$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(abs($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(abs());

?>
===Done===
--EXPECTF--
*** Testing abs() : error conditions ***

Too many arguments

Warning: abs() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Too few arguments

Warning: abs() expects exactly 1 parameter, 0 given in %s on line %d
NULL
===Done===
