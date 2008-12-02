--TEST--
Test array_intersect_ukey() function : usage variation - Passing non-existing function name to callback
--FILE--
<?php
/* Prototype  : array array_intersect_ukey(array arr1, array arr2 [, array ...], callback key_compare_func)
 * Description: Computes the intersection of arrays using a callback function on the keys for comparison. 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_ukey() : usage variation ***\n";

//Initialise arguments
$array1 = array('blue'  => 1, 'red'  => 2, 'green'  => 3, 'purple' => 4);
$array2 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan'   => 8);

//function name within double quotes
var_dump( array_intersect_ukey($array1, $array2, "unknown_function") );

//function name within single quotes
var_dump( array_intersect_ukey($array1, $array2, 'unknown_function') );

//function name without quotes
var_dump( array_intersect_ukey($array1, $array2, unknown_function) );
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_ukey() : usage variation ***

Warning: array_intersect_ukey(): Not a valid callback unknown_function in %s on line %d
NULL

Warning: array_intersect_ukey(): Not a valid callback unknown_function in %s on line %d
NULL

Notice: Use of undefined constant unknown_function - assumed 'unknown_function' in %s on line %d

Warning: array_intersect_ukey(): Not a valid callback unknown_function in %s on line %d
NULL
===DONE===