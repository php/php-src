--TEST--
Test ctype_print() function : error conditions - incorrect number of args
--FILE--
<?php
/* Prototype  : bool ctype_print(mixed $c)
 * Description: Checks for printable character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass incorrect number of arguments to ctype_print() to test behaviour
 */

echo "*** Testing ctype_print() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing ctype_print() function with Zero arguments --\n";
var_dump( ctype_print() );

//Test ctype_print with one more than the expected number of arguments
echo "\n-- Testing ctype_print() function with more than expected no. of arguments --\n";
$c = 1;
$extra_arg = 10;
var_dump( ctype_print($c, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing ctype_print() : error conditions ***

-- Testing ctype_print() function with Zero arguments --

Warning: ctype_print() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing ctype_print() function with more than expected no. of arguments --

Warning: ctype_print() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
