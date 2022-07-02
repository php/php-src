--TEST--
Test krsort() function : usage variations - sort octal values
--FILE--
<?php
/*
 * testing krsort() by providing array of octal values for $array argument
 * with following flag values:
 *  1.flag value as default
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_NUMERIC - compare items numerically
*/

echo "*** Testing krsort() : usage variations ***\n";

// an array containing unsorted octal values
$unsorted_oct_array = array (
  01235 => 01, 0321 => 02, 0345 => 03, 066 => 04, 0772 => 05,
  077 => 06, -066 => -01, -0345 => -02, 0 => 0
);

echo "\n-- Testing krsort() by supplying octal value array, 'flag' value is default  --\n";
$temp_array = $unsorted_oct_array;
var_dump( krsort($temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing krsort() by supplying octal value array, 'flag' value is SORT_REGULAR  --\n";
$temp_array = $unsorted_oct_array;
var_dump( krsort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing krsort() by supplying octal value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $unsorted_oct_array;
var_dump( krsort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing krsort() : usage variations ***

-- Testing krsort() by supplying octal value array, 'flag' value is default  --
bool(true)
array(9) {
  [669]=>
  int(1)
  [506]=>
  int(5)
  [229]=>
  int(3)
  [209]=>
  int(2)
  [63]=>
  int(6)
  [54]=>
  int(4)
  [0]=>
  int(0)
  [-54]=>
  int(-1)
  [-229]=>
  int(-2)
}

-- Testing krsort() by supplying octal value array, 'flag' value is SORT_REGULAR  --
bool(true)
array(9) {
  [669]=>
  int(1)
  [506]=>
  int(5)
  [229]=>
  int(3)
  [209]=>
  int(2)
  [63]=>
  int(6)
  [54]=>
  int(4)
  [0]=>
  int(0)
  [-54]=>
  int(-1)
  [-229]=>
  int(-2)
}

-- Testing krsort() by supplying octal value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(9) {
  [669]=>
  int(1)
  [506]=>
  int(5)
  [229]=>
  int(3)
  [209]=>
  int(2)
  [63]=>
  int(6)
  [54]=>
  int(4)
  [0]=>
  int(0)
  [-54]=>
  int(-1)
  [-229]=>
  int(-2)
}
Done
