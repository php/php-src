--TEST--
Test ceil() - error conditions - incorrect number of args
--FILE--
<?php
/* Prototype  : float ceil  ( float $value  )
 * Description: Round fractions up.
 * Source code: ext/standard/math.c
 */

echo "*** Testing ceil() :  error conditions ***\n";
$arg_0 = 1.0;
$extra_arg = 1;

echo "\nToo many arguments\n";
var_dump(ceil($arg_0, $extra_arg));

echo "\nToo few arguments\n";
var_dump(ceil());
?>
===Done===
--EXPECTF--
*** Testing ceil() :  error conditions ***

Too many arguments

Warning: ceil() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Too few arguments

Warning: ceil() expects exactly 1 parameter, 0 given in %s on line %d
NULL
===Done===
