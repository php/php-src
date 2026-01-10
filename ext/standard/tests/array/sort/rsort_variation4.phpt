--TEST--
Test rsort() function : usage variations - referenced variables
--FILE--
<?php
/*
 * Test behaviour of rsort() when:
 * 1. passed an array of referenced variables
 * 2. $array_arg is a reference to another array
 * 3. $array_arg is passed by reference
 */

echo "*** Testing rsort() : variation ***\n";

$value1 = 100;
$value2 = 33;
$value3 = 555;

// an array containing integer references
$unsorted_numerics =  array( &$value1 , &$value2, &$value3);

echo "\n-- 'flag' value is default --\n";
$temp_array = $unsorted_numerics;
var_dump( rsort($temp_array) );
var_dump( $temp_array);

echo "\n-- 'flag' = SORT_REGULAR --\n";
$temp_array = &$unsorted_numerics;
var_dump( rsort($temp_array, SORT_REGULAR) );
var_dump( $temp_array);

echo "Done";
?>
--EXPECT--
*** Testing rsort() : variation ***

-- 'flag' value is default --
bool(true)
array(3) {
  [0]=>
  &int(555)
  [1]=>
  &int(100)
  [2]=>
  &int(33)
}

-- 'flag' = SORT_REGULAR --
bool(true)
array(3) {
  [0]=>
  &int(555)
  [1]=>
  &int(100)
  [2]=>
  &int(33)
}
Done
