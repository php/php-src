--TEST--
Test array_udiff_uassoc() function : error conditions 
--FILE--
<?php
/* Prototype  : array array_udiff_uassoc(array arr1, array arr2 [, array ...], callback data_comp_func, callback key_comp_func)
 * Description: Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal. Keys and elements are compared by user supplied functions. 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_udiff_uassoc() : error conditions ***\n";


$arr1 = array(1, 2);
$arr2 = array(1, 2);

include('compare_function.inc');
$data_comp_func = 'compare_function';
$key_comp_func = 'compare_function';
$extra_arg = 10;

//Test array_udiff_uassoc with one more than the expected number of arguments
echo "\n-- Testing array_udiff_uassoc() function with more than expected no. of arguments --\n";
var_dump( array_udiff_uassoc($arr1, $arr2, $data_comp_func, $key_comp_func, $extra_arg) );

// Testing array_udiff_uassoc with one less than the expected number of arguments
echo "\n-- Testing array_udiff_uassoc() function with less than expected no. of arguments --\n";
var_dump( array_udiff_uassoc($arr1, $arr2, $data_comp_func) );

?>
===DONE===
--EXPECTF--
*** Testing array_udiff_uassoc() : error conditions ***

-- Testing array_udiff_uassoc() function with more than expected no. of arguments --

Warning: array_udiff_uassoc(): Not a valid callback 10 in %s on line %d
NULL

-- Testing array_udiff_uassoc() function with less than expected no. of arguments --

Warning: Wrong parameter count for array_udiff_uassoc() in %s on line %d
NULL
===DONE===

