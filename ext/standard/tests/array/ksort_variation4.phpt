--TEST--
Test ksort() function : usage variations - sort bool values
--FILE--
<?php
/* Prototype  : bool ksort ( array &$array [, int $sort_flags] )
 * Description: Sort an array by key, maintaining key to data correlation
 * Source code: ext/standard/array.c
*/

/*
 * testing ksort() by providing array of boolean values for $array argument with following flag values:
 *  1.flag value as defualt
 *  2.SORT_REGULAR - compare items normally
*/

echo "*** Testing ksort() : usage variations ***\n";

// bool value array
$bool_values = array (true => true, false => false, TRUE => TRUE, FALSE => FALSE);

echo "\n-- Testing ksort() by supplying boolean value array, 'flag' value is defualt --\n";
$temp_array = $bool_values;
var_dump(ksort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing ksort() by supplying boolean value array, 'flag' value is SORT_REGULAR --\n";
$temp_array = $bool_values;
var_dump(ksort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "\n-- Testing ksort() by supplying boolean value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $bool_values;
var_dump(ksort($temp_array, SORT_NUMERIC) );
var_dump($temp_array);

echo "\n-- Testing ksort() by supplying boolean value array, 'flag' value is SORT_STRING --\n";
$temp_array = $bool_values;
var_dump(ksort($temp_array, SORT_STRING) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing ksort() : usage variations ***

-- Testing ksort() by supplying boolean value array, 'flag' value is defualt --
bool(true)
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}

-- Testing ksort() by supplying boolean value array, 'flag' value is SORT_REGULAR --
bool(true)
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}

-- Testing ksort() by supplying boolean value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}

-- Testing ksort() by supplying boolean value array, 'flag' value is SORT_STRING --
bool(true)
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}
Done
