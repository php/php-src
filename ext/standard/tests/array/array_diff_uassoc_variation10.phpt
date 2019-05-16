--TEST--
Test array_diff_uassoc() function : usage variation - Passing float indexed array
--FILE--
<?php
/* Prototype  : array array_diff_uassoc(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Computes the difference of arrays with additional index check which is performed by a
 * 				user supplied callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_uassoc() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', 10 => '10', -10 => '-10', 20 =>'20', -20 => '-20');
$float_indx_array = array(0.0 => '0.0', 10.5 => '10.5', -10.5 => '-10.5', 0.5 => '0.5');

echo "\n-- Testing array_diff_key() function with float indexed array --\n";
var_dump( array_diff_uassoc($input_array, $float_indx_array, "strcasecmp") );
var_dump( array_diff_uassoc($float_indx_array, $input_array, "strcasecmp") );

?>
===DONE===
--EXPECT--
*** Testing array_diff_uassoc() : usage variation ***

-- Testing array_diff_key() function with float indexed array --
array(5) {
  [0]=>
  string(1) "0"
  [10]=>
  string(2) "10"
  [-10]=>
  string(3) "-10"
  [20]=>
  string(2) "20"
  [-20]=>
  string(3) "-20"
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
