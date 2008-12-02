--TEST--
Test rsort() function : usage variations - Octal values
--FILE--
<?php
/* Prototype  : bool rsort(array &$array_arg [, int $sort_flags])
 * Description: Sort an array in reverse order 
 * Source code: ext/standard/array.c
 */

/*
 * Pass rsort() an array containing octal values to test behaviour
 */

echo "*** Testing rsort() : variation ***\n";

// an array containing unsorted octal values
$unsorted_oct_array = array(01235, 0321, 0345, 066, 0772, 077, -066, -0345, 0);

echo "\n-- Sort flag = default  --\n";
$temp_array = $unsorted_oct_array;
var_dump(rsort($temp_array) );
var_dump($temp_array);

echo "\n-- Sort flag = SORT_REGULAR  --\n";
$temp_array = $unsorted_oct_array;
var_dump(rsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "\n-- Sort flag = SORT_NUMERIC  --\n";
$temp_array = $unsorted_oct_array;
var_dump(rsort($temp_array, SORT_NUMERIC) );
var_dump($temp_array);

echo "Done";
?>

--EXPECTF--
*** Testing rsort() : variation ***

-- Sort flag = default  --
bool(true)
array(9) {
  [0]=>
  int(669)
  [1]=>
  int(506)
  [2]=>
  int(229)
  [3]=>
  int(209)
  [4]=>
  int(63)
  [5]=>
  int(54)
  [6]=>
  int(0)
  [7]=>
  int(-54)
  [8]=>
  int(-229)
}

-- Sort flag = SORT_REGULAR  --
bool(true)
array(9) {
  [0]=>
  int(669)
  [1]=>
  int(506)
  [2]=>
  int(229)
  [3]=>
  int(209)
  [4]=>
  int(63)
  [5]=>
  int(54)
  [6]=>
  int(0)
  [7]=>
  int(-54)
  [8]=>
  int(-229)
}

-- Sort flag = SORT_NUMERIC  --
bool(true)
array(9) {
  [0]=>
  int(669)
  [1]=>
  int(506)
  [2]=>
  int(229)
  [3]=>
  int(209)
  [4]=>
  int(63)
  [5]=>
  int(54)
  [6]=>
  int(0)
  [7]=>
  int(-54)
  [8]=>
  int(-229)
}
Done