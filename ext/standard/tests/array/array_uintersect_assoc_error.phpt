--TEST--
Test array_uintersect_assoc() function : error conditions
--FILE--
<?php
/* Prototype  : array array_uintersect_assoc(array arr1, array arr2 [, array ...], callback data_compare_func)
 * Description: U
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_uintersect_assoc() : error conditions ***\n";

//Test array_uintersect_assoc with one more than the expected number of arguments
echo "\n-- Testing array_uintersect_assoc() function with more than expected no. of arguments --\n";
$arr1 = array(1, 2);
$arr2 = array(1, 2);
include('compare_function.inc');
$data_compare_function = 'compare_function';

$extra_arg = 10;
var_dump( array_uintersect_assoc($arr1, $arr2, $data_compare_function, $extra_arg) );

// Testing array_uintersect_assoc with one less than the expected number of arguments
echo "\n-- Testing array_uintersect_assoc() function with less than expected no. of arguments --\n";
$arr1 = array(1, 2);
$arr2 = array(1, 2);
var_dump( array_uintersect_assoc($arr1, $arr2) );

?>
===DONE===
--EXPECTF--
*** Testing array_uintersect_assoc() : error conditions ***

-- Testing array_uintersect_assoc() function with more than expected no. of arguments --

Warning: array_uintersect_assoc() expects parameter 4 to be a valid callback, no array or string given in %sarray_uintersect_assoc_error.php on line %d
NULL

-- Testing array_uintersect_assoc() function with less than expected no. of arguments --

Warning: array_uintersect_assoc(): at least 3 parameters are required, 2 given in %sarray_uintersect_assoc_error.php on line %d
NULL
===DONE===
