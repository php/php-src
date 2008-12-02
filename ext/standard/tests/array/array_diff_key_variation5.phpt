--TEST--
Test array_diff_key() function : usage variation - Passing float indexed array
--FILE--
<?php
/* Prototype  : array array_diff_key(array arr1, array arr2 [, array ...])
 * Description: Returns the entries of arr1 that have keys which are not present in any of the others arguments. 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_key() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', 10 => '10', -10 => '-10', 20 =>'20', -20 => '-20'); 
$float_indx_array = array(0.0 => '0.0', 10.5 => '10.5', -10.5 => '-10.5', 0.5 => '0.5'); 

echo "\n-- Testing array_diff_key() function with float indexed array --\n";
// loop through each element of the array for arr1
var_dump( array_diff_key($input_array, $float_indx_array) );
var_dump( array_diff_key($float_indx_array, $input_array) );
?>
===DONE===
--EXPECTF--
*** Testing array_diff_key() : usage variation ***

-- Testing array_diff_key() function with float indexed array --
array(2) {
  [20]=>
  string(2) "20"
  [-20]=>
  string(3) "-20"
}
array(0) {
}
===DONE===