--TEST--
Test array_diff_uassoc() function : usage variation - Passing boolean indexed array
--FILE--
<?php
/* Prototype  : array array_diff_uassoc(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Computes the difference of arrays with additional index check which is performed by a
 * 				user supplied callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_uassoc() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', 1 => '1', -10 => '-10', 'true' => 1, 'false' => 0);
$boolean_indx_array = array(true => 'boolt', false => 'boolf', TRUE => 'boolT', FALSE => 'boolF');

echo "\n-- Testing array_diff_key() function with float indexed array --\n";
var_dump( array_diff_uassoc($input_array, $boolean_indx_array, "strcasecmp") );
var_dump( array_diff_uassoc($boolean_indx_array, $input_array, "strcasecmp") );

?>
===DONE===
--EXPECTF--
*** Testing array_diff_uassoc() : usage variation ***

-- Testing array_diff_key() function with float indexed array --
array(5) {
  [0]=>
  string(1) "0"
  [1]=>
  string(1) "1"
  [-10]=>
  string(3) "-10"
  ["true"]=>
  int(1)
  ["false"]=>
  int(0)
}
array(2) {
  [1]=>
  string(5) "boolT"
  [0]=>
  string(5) "boolF"
}
===DONE===
