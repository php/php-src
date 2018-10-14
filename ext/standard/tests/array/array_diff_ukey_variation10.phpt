--TEST--
Test array_diff_ukey() function : usage variation - Passing non-existing function name to callback
--FILE--
<?php
/* Prototype  : array array_diff_ukey(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Returns the entries of arr1 that have keys which are not present in any of the others arguments.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_ukey() : usage variation ***\n";

//Initialize variables
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array2 = array("a" => "green", "yellow", "red");

//function name within double quotes
var_dump( array_diff_ukey($array1, $array1, "unknown_function") );

//function name within single quotes
var_dump( array_diff_ukey($array1, $array1, 'unknown_function') );

//function name without quotes
var_dump( array_diff_ukey($array1, $array1, unknown_function) );

?>
===DONE===
--EXPECTF--
*** Testing array_diff_ukey() : usage variation ***

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, function 'unknown_function' not found or invalid function name in %s on line %d
NULL

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, function 'unknown_function' not found or invalid function name in %s on line %d
NULL

Warning: Use of undefined constant unknown_function - assumed 'unknown_function' (this will throw an Error in a future version of PHP) in %s on line %d

Warning: array_diff_ukey() expects parameter 3 to be a valid callback, function 'unknown_function' not found or invalid function name in %s on line %d
NULL
===DONE===
