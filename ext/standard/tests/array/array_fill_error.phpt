--TEST--
Test array_fill() function : error conditions 
--FILE--
<?php
/* Prototype  : array array_fill(int $start_key, int $num, mixed $val)
 * Description: Create an array containing num elements starting with index start_key each initialized to val 
 * Source code: ext/standard/array.c
 */


echo "*** Testing array_fill() : error conditions ***\n";

// Zero arguments
echo "-- Testing array_fill() function with Zero arguments --\n";
var_dump( array_fill() );

// More than  expected number of arguments
echo "-- Testing array_fill() function with more than expected no. of arguments --\n";
$start_key = 0;
$num = 2;
$val = 1;
$extra_arg = 10;
var_dump( array_fill($start_key, $num,$val, $extra_arg) );

// Less than the expected number of arguments
echo "-- Testing array_fill() function with less than expected no. of arguments --\n";
$start_key = 0;
$num = 2;
var_dump( array_fill($start_key, $num) );

//calling array_fill with negative values for 'num' parameter
$num = -1;
var_dump( array_fill($start_key, $num, $val) );

//callin array_fill with 'num' equal to zero value
$num = 0;
var_dump( array_fill($start_key, $num, $val) );

echo "Done";
?>
--EXPECTF--
*** Testing array_fill() : error conditions ***
-- Testing array_fill() function with Zero arguments --

Warning: Wrong parameter count for array_fill() in %s on line %d
NULL
-- Testing array_fill() function with more than expected no. of arguments --

Warning: Wrong parameter count for array_fill() in %s on line %d
NULL
-- Testing array_fill() function with less than expected no. of arguments --

Warning: Wrong parameter count for array_fill() in %s on line %d
NULL

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)

Warning: array_fill(): Number of elements must be positive in %s on line %d
bool(false)
Done
