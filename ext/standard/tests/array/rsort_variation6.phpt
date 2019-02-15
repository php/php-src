--TEST--
Test rsort() function : usage variations - Hexadecimal vales
--FILE--
<?php
/* Prototype  : bool rsort(array &$array_arg [, int $sort_flags])
 * Description: Sort an array in reverse order
 * Source code: ext/standard/array.c
 */

/*
 * Pass rsort() an array of hexadecimal values to test behaviour
 */

echo "*** Testing rsort() : variation ***\n";

// an array contains unsorted hexadecimal values
$unsorted_hex_array = array(0x1AB, 0xFFF, 0xF, 0xFF, 0x2AA, 0xBB, 0x1ab, 0xff, -0xFF, 0, -0x2aa);

echo "\n-- 'flag' value is defualt  --\n";
$temp_array = $unsorted_hex_array;
var_dump(rsort($temp_array) );
var_dump($temp_array);

echo "\n-- 'flag' value is SORT_REGULAR  --\n";
$temp_array = $unsorted_hex_array;
var_dump(rsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "\n-- 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $unsorted_hex_array;
var_dump(rsort($temp_array, SORT_NUMERIC) );
var_dump($temp_array);

echo "Done";
?>
--EXPECT--
*** Testing rsort() : variation ***

-- 'flag' value is defualt  --
bool(true)
array(11) {
  [0]=>
  int(4095)
  [1]=>
  int(682)
  [2]=>
  int(427)
  [3]=>
  int(427)
  [4]=>
  int(255)
  [5]=>
  int(255)
  [6]=>
  int(187)
  [7]=>
  int(15)
  [8]=>
  int(0)
  [9]=>
  int(-255)
  [10]=>
  int(-682)
}

-- 'flag' value is SORT_REGULAR  --
bool(true)
array(11) {
  [0]=>
  int(4095)
  [1]=>
  int(682)
  [2]=>
  int(427)
  [3]=>
  int(427)
  [4]=>
  int(255)
  [5]=>
  int(255)
  [6]=>
  int(187)
  [7]=>
  int(15)
  [8]=>
  int(0)
  [9]=>
  int(-255)
  [10]=>
  int(-682)
}

-- 'flag' value is SORT_NUMERIC  --
bool(true)
array(11) {
  [0]=>
  int(4095)
  [1]=>
  int(682)
  [2]=>
  int(427)
  [3]=>
  int(427)
  [4]=>
  int(255)
  [5]=>
  int(255)
  [6]=>
  int(187)
  [7]=>
  int(15)
  [8]=>
  int(0)
  [9]=>
  int(-255)
  [10]=>
  int(-682)
}
Done
