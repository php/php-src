--TEST--
Test array_count_values() function : Invalid parameters 
--FILE--
<?php
/* Prototype  : proto array array_count_values(array input)
 * Description: Return the value as key and the frequency of that value in input as value 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

/*
 * Test for handling of incorrect parameters.
 */

echo "*** Testing array_count_values() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_count_values() function with Zero arguments --\n";
var_dump( array_count_values() );

//Test array_count_values with one more than the expected number of arguments
echo "\n-- Testing array_count_values() function with more than expected no. of arguments --\n";
$input = array(1, 2);
$extra_arg = 10;
var_dump( array_count_values($input, $extra_arg) );

//Test array_count_values with integer arguments
echo "\n-- Testing array_count_values() function integer arguments --\n";
var_dump( array_count_values(1 ));

echo "Done";
?>
--EXPECTF--
*** Testing array_count_values() : error conditions ***

-- Testing array_count_values() function with Zero arguments --

Warning: Wrong parameter count for array_count_values() in %sarray_count_values_error.php on line 16
NULL

-- Testing array_count_values() function with more than expected no. of arguments --

Warning: Wrong parameter count for array_count_values() in %sarray_count_values_error.php on line 22
NULL

-- Testing array_count_values() function integer arguments --

Warning: array_count_values(): The argument should be an array in %sarray_count_values_error.php on line 26
NULL
Done
