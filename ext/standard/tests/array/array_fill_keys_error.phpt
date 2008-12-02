--TEST--
Test array_fill_keys() function : error conditions 
--FILE--
<?php
/* Prototype  : proto array array_fill_keys(array keys, mixed val)
 * Description: Create an array using the elements of the first parameter as keys each initialized to val 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

/*
 * add a comment here to say what the test is supposed to do
 */

echo "*** Testing array_fill_keys() : error conditions ***\n";

$keys = array(1, 2);
$val = 1;
$extra_arg = 10;

echo "\n-- Testing array_fill_keys() function with more than expected no. of arguments --\n";
var_dump( array_fill_keys($keys, $val, $extra_arg) );

echo "\n-- Testing array_fill_keys() function with less than expected no. of arguments --\n";
var_dump( array_fill_keys($keys) );

echo "\n-- Testing array_fill_keys() function with no argumets --\n";
var_dump( array_fill_keys() );

echo "Done";
?>
--EXPECTF--
*** Testing array_fill_keys() : error conditions ***

-- Testing array_fill_keys() function with more than expected no. of arguments --

Warning: array_fill_keys() expects exactly 2 parameters, 3 given in %sarray_fill_keys_error.php on line %d
NULL

-- Testing array_fill_keys() function with less than expected no. of arguments --

Warning: array_fill_keys() expects exactly 2 parameters, 1 given in %sarray_fill_keys_error.php on line %d
NULL

-- Testing array_fill_keys() function with no argumets --

Warning: array_fill_keys() expects exactly 2 parameters, 0 given in %sarray_fill_keys_error.php on line %d
NULL
Done
