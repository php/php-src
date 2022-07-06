--TEST--
Test asort() function : usage variations - sort octal values
--FILE--
<?php
/*
 * testing asort() by providing different octal array for $array argument with following flag values
 * 1.flag value as default
 * 2.SORT_REGULAR - compare items normally
 * 3.SORT_NUMERIC - compare items numerically
*/

echo "*** Testing asort() : usage variations ***\n";

// an array contains unsorted octal values
$unsorted_oct_array = array (
   01235 => 01235, 0321 => 0321, 0345 => 0345, 066 => 066, 0772 => 0772,
   077 => 077, -066 => -066, -0345 => -0345, 0 => 0
);

echo "\n-- Testing asort() by supplying octal value array, 'flag' value is default  --\n";
$temp_array = $unsorted_oct_array;
var_dump( asort($temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing asort() by supplying octal value array, 'flag' value is SORT_REGULAR  --\n";
$temp_array = $unsorted_oct_array;
var_dump( asort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing asort() by supplying octal value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $unsorted_oct_array;
var_dump( asort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing asort() : usage variations ***

-- Testing asort() by supplying octal value array, 'flag' value is default  --
bool(true)
array(9) {
  [-229]=>
  int(-229)
  [-54]=>
  int(-54)
  [0]=>
  int(0)
  [54]=>
  int(54)
  [63]=>
  int(63)
  [209]=>
  int(209)
  [229]=>
  int(229)
  [506]=>
  int(506)
  [669]=>
  int(669)
}

-- Testing asort() by supplying octal value array, 'flag' value is SORT_REGULAR  --
bool(true)
array(9) {
  [-229]=>
  int(-229)
  [-54]=>
  int(-54)
  [0]=>
  int(0)
  [54]=>
  int(54)
  [63]=>
  int(63)
  [209]=>
  int(209)
  [229]=>
  int(229)
  [506]=>
  int(506)
  [669]=>
  int(669)
}

-- Testing asort() by supplying octal value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(9) {
  [-229]=>
  int(-229)
  [-54]=>
  int(-54)
  [0]=>
  int(0)
  [54]=>
  int(54)
  [63]=>
  int(63)
  [209]=>
  int(209)
  [229]=>
  int(229)
  [506]=>
  int(506)
  [669]=>
  int(669)
}
Done
