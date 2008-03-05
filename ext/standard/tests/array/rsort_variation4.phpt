--TEST--
Test rsort() function : usage variations - referenced variables
--INI--
allow_call_time_pass_reference=on
--FILE--
<?php
/* Prototype  : bool rsort(array &$array_arg [, int $sort_flags])
 * Description: Sort an array in reverse order 
 * Source code: ext/standard/array.c
 */

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

echo "\n-- 'flag' value is defualt --\n";
$temp_array = $unsorted_numerics;
var_dump( rsort($temp_array) );
var_dump( $temp_array);

echo "\n-- 'flag' = SORT_REGULAR --\n";
$temp_array = &$unsorted_numerics;
var_dump( rsort($temp_array, SORT_REGULAR) );
var_dump( $temp_array);

echo "\n-- 'flag' = SORT_NUMERIC --\n";
$temp_array = $unsorted_numerics;
var_dump( rsort(&$temp_array, SORT_NUMERIC) );
var_dump( $temp_array);

echo "Done";
?>
--EXPECTF--
*** Testing rsort() : variation ***

-- 'flag' value is defualt --
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

-- 'flag' = SORT_NUMERIC --
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
