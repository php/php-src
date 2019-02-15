--TEST--
Test array_pad() function : error conditions
--FILE--
<?php
/* Prototype  : array array_pad(array $input, int $pad_size, mixed $pad_value)
 * Description: Returns a copy of input array padded with pad_value to size pad_size
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_pad() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_pad() function with Zero arguments --\n";
var_dump( array_pad() );

//Test array_pad with one more than the expected number of arguments
echo "\n-- Testing array_pad() function with more than expected no. of arguments --\n";
$input = array(1, 2);
$pad_size = 10;
$pad_value = 1;
$extra_arg = 10;
var_dump( array_pad($input, $pad_size, $pad_value, $extra_arg) );

// Testing array_pad with less than the expected number of arguments
echo "\n-- Testing array_pad() function with less than expected no. of arguments --\n";
$input = array(1, 2);
$pad_size = 10;
var_dump( array_pad($input, $pad_size) );
var_dump( array_pad($input) );

echo "Done";
?>
--EXPECTF--
*** Testing array_pad() : error conditions ***

-- Testing array_pad() function with Zero arguments --

Warning: array_pad() expects exactly 3 parameters, 0 given in %s on line %d
NULL

-- Testing array_pad() function with more than expected no. of arguments --

Warning: array_pad() expects exactly 3 parameters, 4 given in %s on line %d
NULL

-- Testing array_pad() function with less than expected no. of arguments --

Warning: array_pad() expects exactly 3 parameters, 2 given in %s on line %d
NULL

Warning: array_pad() expects exactly 3 parameters, 1 given in %s on line %d
NULL
Done
