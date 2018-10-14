--TEST--
Test count() function : error conditions - pass incorrect number of args
--FILE--
<?php
/* Prototype  : int count(mixed var [, int mode])
 * Description: Count the number of elements in a variable (usually an array)
 * Source code: ext/standard/array.c
 */

/*
 * Pass incorrect number of arguments to count() to test behaviour
 */

echo "*** Testing count() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing count() function with Zero arguments --\n";
var_dump( count() );

//Test count with one more than the expected number of arguments
echo "\n-- Testing count() function with more than expected no. of arguments --\n";
$var = 1;
$mode = 10;
$extra_arg = 10;
var_dump( count($var, $mode, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing count() : error conditions ***

-- Testing count() function with Zero arguments --

Warning: count() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing count() function with more than expected no. of arguments --

Warning: count() expects at most 2 parameters, 3 given in %s on line %d
NULL
Done
