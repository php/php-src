--TEST--
Test array_pad() function : basic functionality 
--FILE--
<?php
/* Prototype  : array array_pad(array $input, int $pad_size, mixed $pad_value)
 * Description: Returns a copy of input array padded with pad_value to size pad_size 
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_pad() : basic functionality ***\n";

// Initialise $input and $pad_value arguments
$input = array(1, 2, 3);
$pad_value = -3;

// positive $pad_size
echo "-- Positive pad_size --\n";
var_dump( array_pad($input, 8, $pad_value) );

// negative $pad_size
echo "-- Negative pad_size --\n";
var_dump( array_pad($input, -8, $pad_value) );

// $pad_size less than array size, no padding expected
echo "-- Pad_size lesser than array_size --\n";
var_dump( array_pad($input, 2, $pad_value) );

// $pad_size equal to array size, no padding expected
echo "-- Pad_size equal to array_size --\n";
var_dump( array_pad($input, 3, $pad_value) );

echo "Done";
?>
--EXPECTF--
*** Testing array_pad() : basic functionality ***
-- Positive pad_size --
array(8) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(-3)
  [4]=>
  int(-3)
  [5]=>
  int(-3)
  [6]=>
  int(-3)
  [7]=>
  int(-3)
}
-- Negative pad_size --
array(8) {
  [0]=>
  int(-3)
  [1]=>
  int(-3)
  [2]=>
  int(-3)
  [3]=>
  int(-3)
  [4]=>
  int(-3)
  [5]=>
  int(1)
  [6]=>
  int(2)
  [7]=>
  int(3)
}
-- Pad_size lesser than array_size --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
-- Pad_size equal to array_size --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Done
