--TEST--
Test array_sum() function : error conditions 
--FILE--
<?php
/* Prototype  : mixed array_sum(array &input)
 * Description: Returns the sum of the array entries 
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_sum() : error conditions ***\n";

// Zero arguments
echo "-- Testing array_sum() function with zero arguments --\n";
var_dump( array_sum() );

// One more than the expected number of arguments
echo "-- Testing array_sum() function with more than expected no. of arguments --\n";
$input = array(1, 2, 3, 4);
$extra_arg = 10;
var_dump( array_sum($input, $extra_arg) );

echo "Done"
?>
--EXPECTF--
*** Testing array_sum() : error conditions ***
-- Testing array_sum() function with zero arguments --

Warning: array_sum() expects exactly 1 parameter, 0 given in %s on line %d
NULL
-- Testing array_sum() function with more than expected no. of arguments --

Warning: array_sum() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
