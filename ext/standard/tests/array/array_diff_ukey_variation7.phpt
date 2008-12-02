--TEST--
Test array_diff_ukey() function : usage variation - Passing float indexed array
--FILE--
<?php
/* Prototype  : array array_diff_ukey(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Returns the entries of arr1 that have keys which are not present in any of the others arguments. 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_ukey() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', 10 => '10', -10 => '-10', 20 =>'20'); 
$float_indx_array = array(0.0 => '0.0', 10.5 => '10.5', -30.5 => '-30.5'); 

function key_compare_func($key1, $key2)
{
  return strcasecmp($key1, $key2);
}

echo "\n-- Testing array_diff_ukey() function with float indexed array --\n";

var_dump( array_diff_ukey($float_indx_array, $input_array, 'key_compare_func') );
var_dump( array_diff_ukey($input_array, $float_indx_array, 'key_compare_func') );

?>
===DONE===
--EXPECTF--
*** Testing array_diff_ukey() : usage variation ***

-- Testing array_diff_ukey() function with float indexed array --
array(1) {
  [-30]=>
  string(5) "-30.5"
}
array(2) {
  [-10]=>
  string(3) "-10"
  [20]=>
  string(2) "20"
}
===DONE===
