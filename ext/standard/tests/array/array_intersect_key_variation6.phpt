--TEST--
Test array_intersect_key() function : usage variation - Passing boolean indexed array
--FILE--
<?php
/* Prototype  : array array_intersect_key(array arr1, array arr2 [, array ...])
 * Description: Returns the entries of arr1 that have keys which are present in all the other arguments. 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_key() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$input_array = array(0 => '0', 1 => '1' , -10 => '-10'); 
$boolean_indx_array = array(true => 'boolt', false => 'boolf', TRUE => 'boolT', FALSE => 'boolF');

echo "\n-- Testing array_intersect_key() function with boolean indexed array --\n";
var_dump( array_intersect_key($input_array, $boolean_indx_array) );
var_dump( array_intersect_key($boolean_indx_array,$input_array ) );
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_key() : usage variation ***

-- Testing array_intersect_key() function with boolean indexed array --
array(2) {
  [0]=>
  string(1) "0"
  [1]=>
  string(1) "1"
}
array(2) {
  [1]=>
  string(5) "boolT"
  [0]=>
  string(5) "boolF"
}
===DONE===
