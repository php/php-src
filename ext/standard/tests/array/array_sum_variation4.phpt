--TEST--
Test array_sum() function : usage variations - array with duplicate values
--FILE--
<?php
/* Prototype  : mixed array_sum(array $input)
 * Description: Returns the sum of the array entries
 * Source code: ext/standard/array.c
*/

/*
* Checking array_sum() with integer and float array containing duplicate values
*/

echo "*** Testing array_sum() : array with duplicate values ***\n";

// integer array with duplicate values
$int_input = array( 2, 5, 7, 5, 0, -4, 2, 100);
echo "-- With integer array --\n";
var_dump( array_sum($int_input) );

// float array with duplicate values
$float_input = array( 2.3, 1.9, -4.1, 0.5, 1.9, -4.1, 3.6, 0.5);
echo "-- With float array --\n";
var_dump( array_sum($float_input) );

echo "Done"
?>
--EXPECT--
*** Testing array_sum() : array with duplicate values ***
-- With integer array --
int(117)
-- With float array --
float(2.5)
Done
