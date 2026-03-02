--TEST--
Test sort() function : usage variations - sort octal values
--FILE--
<?php
/*
 * testing sort() by providing different octal array for $array argument
 * with following flag values
 * 1.flag value as default
 * 2.SORT_REGULAR - compare items normally
 * 3.SORT_NUMERIC - compare items numerically
*/

echo "*** Testing sort() : usage variations ***\n";

// an array containing unsorted octal values
$unsorted_oct_array = array(01235, 0321, 0345, 066, 0772, 077, -066, -0345, 0);

echo "\n-- Testing sort() by supplying octal value array, 'flag' value is default  --\n";
$temp_array = $unsorted_oct_array;
var_dump(sort($temp_array) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing sort() by supplying octal value array, 'flag' value is SORT_REGULAR  --\n";
$temp_array = $unsorted_oct_array;
var_dump(sort($temp_array, SORT_REGULAR) ); // expecting : bool(true)
var_dump($temp_array);

echo "\n-- Testing sort() by supplying octal value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $unsorted_oct_array;
var_dump(sort($temp_array, SORT_NUMERIC) ); // expecting : bool(true)
var_dump($temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing sort() : usage variations ***

-- Testing sort() by supplying octal value array, 'flag' value is default  --
bool(true)
array(9) {
  [0]=>
  int(-229)
  [1]=>
  int(-54)
  [2]=>
  int(0)
  [3]=>
  int(54)
  [4]=>
  int(63)
  [5]=>
  int(209)
  [6]=>
  int(229)
  [7]=>
  int(506)
  [8]=>
  int(669)
}

-- Testing sort() by supplying octal value array, 'flag' value is SORT_REGULAR  --
bool(true)
array(9) {
  [0]=>
  int(-229)
  [1]=>
  int(-54)
  [2]=>
  int(0)
  [3]=>
  int(54)
  [4]=>
  int(63)
  [5]=>
  int(209)
  [6]=>
  int(229)
  [7]=>
  int(506)
  [8]=>
  int(669)
}

-- Testing sort() by supplying octal value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(9) {
  [0]=>
  int(-229)
  [1]=>
  int(-54)
  [2]=>
  int(0)
  [3]=>
  int(54)
  [4]=>
  int(63)
  [5]=>
  int(209)
  [6]=>
  int(229)
  [7]=>
  int(506)
  [8]=>
  int(669)
}
Done
