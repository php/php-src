--TEST--
Test array_intersect_key() function : usage variation - Passing float indexed array
--FILE--
<?php
/* Prototype  : array array_intersect_key(array arr1, array arr2 [, array ...])
 * Description: Returns the entries of arr1 that have keys which are present in all the other arguments.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_key() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', 10 => '10' , -10 => '-10');
$float_indx_array = array(0.0 => '0.0', 10.5 => '10.5' , -10.5 => '-10.5', 0.5 => '0.5');

echo "\n-- Testing array_intersect_key() function with float indexed array --\n";
var_dump( array_intersect_key($input_array, $float_indx_array) );
var_dump( array_intersect_key($float_indx_array,$input_array ) );
?>
===DONE===
--EXPECT--
*** Testing array_intersect_key() : usage variation ***

-- Testing array_intersect_key() function with float indexed array --
array(3) {
  [0]=>
  string(1) "0"
  [10]=>
  string(2) "10"
  [-10]=>
  string(3) "-10"
}
array(3) {
  [0]=>
  string(3) "0.5"
  [10]=>
  string(4) "10.5"
  [-10]=>
  string(5) "-10.5"
}
===DONE===
