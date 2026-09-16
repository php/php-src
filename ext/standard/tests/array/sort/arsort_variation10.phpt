--TEST--
Test arsort() function : usage variations - sort octal values
--FILE--
<?php
/*
 * testing arsort() by providing different octal array for $array argument with following flag values
 * 1.flag value as default
 * 2.SORT_REGULAR - compare items normally
 * 3.SORT_NUMERIC - compare items numerically
*/

echo "*** Testing arsort() : usage variations ***\n";

// an array contains unsorted octal values
$unsorted_oct_array = array (
   01235 => 01235, 0321 => 0321, 0345 => 0345, 066 => 066, 0772 => 0772,
   077 => 077, -066 => -066, -0345 => -0345, 0 => 0
);

echo "\n-- Testing arsort() by supplying octal value array, 'flag' value is default  --\n";
$temp_array = $unsorted_oct_array;
var_dump( arsort($temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing arsort() by supplying octal value array, 'flag' value is SORT_REGULAR  --\n";
$temp_array = $unsorted_oct_array;
var_dump( arsort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing arsort() by supplying octal value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $unsorted_oct_array;
var_dump( arsort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing arsort() : usage variations ***

-- Testing arsort() by supplying octal value array, 'flag' value is default  --
bool(true)
array(9) {
  [669]=>
  int(669)
  [506]=>
  int(506)
  [229]=>
  int(229)
  [209]=>
  int(209)
  [63]=>
  int(63)
  [54]=>
  int(54)
  [0]=>
  int(0)
  [-54]=>
  int(-54)
  [-229]=>
  int(-229)
}

-- Testing arsort() by supplying octal value array, 'flag' value is SORT_REGULAR  --
bool(true)
array(9) {
  [669]=>
  int(669)
  [506]=>
  int(506)
  [229]=>
  int(229)
  [209]=>
  int(209)
  [63]=>
  int(63)
  [54]=>
  int(54)
  [0]=>
  int(0)
  [-54]=>
  int(-54)
  [-229]=>
  int(-229)
}

-- Testing arsort() by supplying octal value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(9) {
  [669]=>
  int(669)
  [506]=>
  int(506)
  [229]=>
  int(229)
  [209]=>
  int(209)
  [63]=>
  int(63)
  [54]=>
  int(54)
  [0]=>
  int(0)
  [-54]=>
  int(-54)
  [-229]=>
  int(-229)
}
Done
