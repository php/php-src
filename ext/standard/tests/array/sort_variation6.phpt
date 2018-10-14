--TEST--
Test sort() function : usage variations - sort hexadecimal values
--FILE--
<?php
/* Prototype  : bool sort ( array &$array [, int $sort_flags] )
 * Description: This function sorts an array.
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing sort() by providing different hexa-decimal array for $array argument with following flag values
 * flag  value as defualt
 * SORT_REGULAR - compare items normally
 * SORT_NUMERIC - compare items numerically
*/

echo "*** Testing sort() : usage variations ***\n";

// an array contains unsorted hexadecimal values
$unsorted_hex_array = array(0x1AB, 0xFFF, 0xF, 0xFF, 0x2AA, 0xBB, 0x1ab, 0xff, -0xFF, 0, -0x2aa);

echo "\n-- Testing sort() by supplying hexadecimal value array, 'flag' value is defualt  --\n";
$temp_array = $unsorted_hex_array;
var_dump(sort($temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing sort() by supplying hexadecimal value array, 'flag' value is SORT_REGULAR  --\n";
$temp_array = $unsorted_hex_array;
var_dump(sort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing sort() by supplying hexadecimal value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $unsorted_hex_array;
var_dump(sort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing sort() : usage variations ***

-- Testing sort() by supplying hexadecimal value array, 'flag' value is defualt  --
bool(true)
array(11) {
  [0]=>
  int(-682)
  [1]=>
  int(-255)
  [2]=>
  int(0)
  [3]=>
  int(15)
  [4]=>
  int(187)
  [5]=>
  int(255)
  [6]=>
  int(255)
  [7]=>
  int(427)
  [8]=>
  int(427)
  [9]=>
  int(682)
  [10]=>
  int(4095)
}

-- Testing sort() by supplying hexadecimal value array, 'flag' value is SORT_REGULAR  --
bool(true)
array(11) {
  [0]=>
  int(-682)
  [1]=>
  int(-255)
  [2]=>
  int(0)
  [3]=>
  int(15)
  [4]=>
  int(187)
  [5]=>
  int(255)
  [6]=>
  int(255)
  [7]=>
  int(427)
  [8]=>
  int(427)
  [9]=>
  int(682)
  [10]=>
  int(4095)
}

-- Testing sort() by supplying hexadecimal value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(11) {
  [0]=>
  int(-682)
  [1]=>
  int(-255)
  [2]=>
  int(0)
  [3]=>
  int(15)
  [4]=>
  int(187)
  [5]=>
  int(255)
  [6]=>
  int(255)
  [7]=>
  int(427)
  [8]=>
  int(427)
  [9]=>
  int(682)
  [10]=>
  int(4095)
}
Done
