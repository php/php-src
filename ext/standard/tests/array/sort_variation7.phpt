--TEST--
Test sort() function : usage variations - sort boolean values
--FILE--
<?php
/* Prototype  : bool sort ( array &$array [, int $sort_flags] )
 * Description: This function sorts an array.
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing sort() by providing bool value array for $array argument with following flag values.
 * flag  value as defualt
 * SORT_REGULAR - compare items normally
*/

echo "*** Testing sort() : usage variations ***\n";

// bool value array
$bool_values = array (true, false, TRUE, FALSE);

echo "\n-- Testing sort() by supplying bool value array, 'flag' value is defualt --\n";
$temp_array = $bool_values;
var_dump(sort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing sort() by supplying bool value array, 'flag' value is SORT_REGULAR --\n";
$temp_array = $bool_values;
var_dump(sort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "\n-- Testing sort() by supplying bool value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $bool_values;
var_dump(sort($temp_array, SORT_NUMERIC) );
var_dump($temp_array);

echo "\n-- Testing sort() by supplying bool value array, 'flag' value is SORT_STRING --\n";
$temp_array = $bool_values;
var_dump(sort($temp_array, SORT_STRING) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing sort() : usage variations ***

-- Testing sort() by supplying bool value array, 'flag' value is defualt --
bool(true)
array(4) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  bool(true)
}

-- Testing sort() by supplying bool value array, 'flag' value is SORT_REGULAR --
bool(true)
array(4) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  bool(true)
}

-- Testing sort() by supplying bool value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(4) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  bool(true)
}

-- Testing sort() by supplying bool value array, 'flag' value is SORT_STRING --
bool(true)
array(4) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  bool(true)
}
Done
