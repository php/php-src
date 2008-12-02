--TEST--
Test array_udiff() function : error conditions 
--FILE--
<?php
/* Prototype  : array array_udiff(array arr1, array arr2 [, array ...], callback data_comp_func)
 * Description: Returns the entries of arr1 that have values which are not present in any of the others arguments. Elements are compared by user supplied function. 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_udiff() : error conditions ***\n";

$arr1 = array(1, 2);
$arr2 = array(1, 2);
include('compare_function.inc');
$data_comp_func = 'compare_function';
$extra_arg = 10;


//Test array_udiff with one more than the expected number of arguments
echo "\n-- Testing array_udiff() function with more than expected no. of arguments --\n";
var_dump( array_udiff($arr1, $arr2, $data_comp_func, $extra_arg) );

// Testing array_udiff with one less than the expected number of arguments
echo "\n-- Testing array_udiff() function with less than expected no. of arguments --\n";
var_dump( array_udiff($arr1, $arr2) );

?>
===DONE===
--EXPECTF--
*** Testing array_udiff() : error conditions ***

-- Testing array_udiff() function with more than expected no. of arguments --

Warning: array_udiff() expects parameter 4 to be a valid callback, no array or string given in %sarray_udiff_error.php on line %d
NULL

-- Testing array_udiff() function with less than expected no. of arguments --

Warning: array_udiff(): at least 3 parameters are required, 2 given in %sarray_udiff_error.php on line %d
NULL
===DONE===
