--TEST--
Test ctype_lower() function : error conditions - incorrect number of args
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_lower(mixed $c)
 * Description: Checks for lowercase character(s)
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass incorrect number of arguments to ctype_lower() to test behaviour
 */

echo "*** Testing ctype_lower() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing ctype_lower() function with Zero arguments --\n";
var_dump( ctype_lower() );

//Test ctype_lower with one more than the expected number of arguments
echo "\n-- Testing ctype_lower() function with more than expected no. of arguments --\n";
$c = 1;
$extra_arg = 10;
var_dump( ctype_lower($c, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing ctype_lower() : error conditions ***

-- Testing ctype_lower() function with Zero arguments --

Warning: ctype_lower() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing ctype_lower() function with more than expected no. of arguments --

Warning: ctype_lower() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
