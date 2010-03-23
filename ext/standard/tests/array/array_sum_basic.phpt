--TEST--
Test array_sum() function : basic functionality 
--FILE--
<?php
/* Prototype  : mixed array_sum(array &input)
 * Description: Returns the sum of the array entries 
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_sum() : basic functionality ***\n";

// array with integer values
$input = array(1, 2, 3, 4, 5);
echo "-- array_sum() with integer array entries --\n";
var_dump( array_sum($input) );

// array with float values
$input = array(1.0, 2.2, 3.4, 4.6);
echo "-- array_sum() with float array entries --\n";
var_dump( array_sum($input) );

// array with integer and float values
$input = array(1, 2.3, 4, 0.6, 10);
echo "-- array_sum() with integer/float array entries --\n";
var_dump( array_sum($input) );

echo "Done"
?>
--EXPECTF--
*** Testing array_sum() : basic functionality ***
-- array_sum() with integer array entries --
int(15)
-- array_sum() with float array entries --
float(11.2)
-- array_sum() with integer/float array entries --
float(17.9)
Done
