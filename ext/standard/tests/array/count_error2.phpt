--TEST--
Test count() function : error conditions 
--FILE--
<?php
/* Prototype  : int count(mixed $var[, int $mode] )
 * Description: Counts an elements in an array. If Standard PHP Library is installed, 
 * it will return the properties of an object.
 * Source code: ext/standard/basic_functions.c
 */

// Calling count() with zero and more than expected arguments .

echo "*** Testing count() : error conditions ***\n";

echo "-- Testing count() with zero arguments --\n";
var_dump( count() );
echo "-- Testing count() function with more than two arguments under COUNT_NORMAL mode --\n";
$var = 100;
$extra_arg = 10;;
var_dump( count($var, COUNT_NORMAL, $extra_arg) );
echo "-- Testing count() function with more than two arguments under COUNT_RECURSIVE mode --\n";
var_dump( count($var, COUNT_RECURSIVE, $extra_arg) );

echo "Done";
?>
--EXPECTF--
*** Testing count() : error conditions ***
-- Testing count() with zero arguments --

Warning: count() expects at least 1 parameter, 0 given in %s on line %d
NULL
-- Testing count() function with more than two arguments under COUNT_NORMAL mode --

Warning: count() expects at most 2 parameters, 3 given in %s on line %d
NULL
-- Testing count() function with more than two arguments under COUNT_RECURSIVE mode --

Warning: count() expects at most 2 parameters, 3 given in %s on line %d
NULL
Done
