--TEST--
Test floor() - error conditions - incorrect number of args
--FILE--
<?php
/* Prototype  : float floor  ( float $value  )
 * Description: Round fractions down.
 * Source code: ext/standard/math.c
 */

echo "*** Testing floor() :  error conditions ***\n";
$arg_0 = 1.0;
$extra_arg = 1;

echo "\n-- Too many arguments --\n";
var_dump(floor($arg_0, $extra_arg));

echo "\n-- Too few arguments --\n";
var_dump(floor());
?>
===Done===
--EXPECTF--
*** Testing floor() :  error conditions ***

-- Too many arguments --

Warning: Wrong parameter count for floor() in %s on line %d
NULL

-- Too few arguments --

Warning: Wrong parameter count for floor() in %s on line %d
NULL
===Done===
