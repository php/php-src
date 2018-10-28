--TEST--
Test ctype_digit() function : error conditions - incorrect number of arguments
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_digit(mixed $c)
 * Description: Checks for numeric character(s)
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass an incorrect number of arguments to ctype_digit() to test behaviour
 */

echo "*** Testing ctype_digit() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing ctype_digit() function with Zero arguments --\n";
var_dump( ctype_digit() );

//Test ctype_digit with one more than the expected number of arguments
echo "\n-- Testing ctype_digit() function with more than expected no. of arguments --\n";
$c = 1;
$extra_arg = 10;
var_dump( ctype_digit($c, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing ctype_digit() : error conditions ***

-- Testing ctype_digit() function with Zero arguments --

Warning: ctype_digit() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing ctype_digit() function with more than expected no. of arguments --

Warning: ctype_digit() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
