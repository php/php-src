--TEST--
Test array_diff_key() function : usage variation - Passing boolean indexed array
--FILE--
<?php
/* Prototype  : array array_diff_key(array arr1, array arr2 [, array ...])
 * Description: Returns the entries of arr1 that have keys which are not present in any of the others arguments.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_key() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', 1 => '1', -10 => '-10', 'true' => 1, 'false' => 0);
$boolean_indx_array = array(true => 'boolt', false => 'boolf', TRUE => 'boolT', FALSE => 'boolF');

echo "\n-- Testing array_diff_key() function with boolean indexed array --\n";
// loop through each element of the array for arr1
var_dump( array_diff_key($input_array, $boolean_indx_array) );
var_dump( array_diff_key($boolean_indx_array, $input_array) );
?>
===DONE===
--EXPECTF--
*** Testing array_diff_key() : usage variation ***

-- Testing array_diff_key() function with boolean indexed array --
array(3) {
  [-10]=>
  string(3) "-10"
  ["true"]=>
  int(1)
  ["false"]=>
  int(0)
}
array(0) {
}
===DONE===
