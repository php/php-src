--TEST--
Test round() function :  error conditions - incorrect number of args
--FILE--
<?php
/* Prototype  : float round  ( float $val  [, int $precision  ] )
 * Description: Returns the rounded value of val  to specified precision (number of digits
 * after the decimal point)
 * Source code: ext/standard/math.c
 */
 
/*
 * Pass incorrect number of arguments to round() to test behaviour
 */
 
echo "*** Testing round() : error conditions ***\n";

echo "\n-- Wrong nmumber of arguments --\n";
var_dump(round());
var_dump(round(500, 10, true));

?>
===Done===
--EXPECTF--
*** Testing round() : error conditions ***

-- Wrong nmumber of arguments --

Warning: round() expects at least 1 parameter, 0 given in %s on line %d
NULL
float(500)
===Done===
