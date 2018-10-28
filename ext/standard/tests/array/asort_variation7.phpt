--TEST--
Test asort() function : usage variations - sort bool values
--FILE--
<?php
/* Prototype  : bool asort ( array &$array [, int $sort_flags] )
 * Description: This function asorts an array.
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing asort() by providing bool value array for $array argument with following flag values.
 * flag value as defualt
 * SORT_REGULAR - compare items normally
*/

echo "*** Testing asort() : usage variations ***\n";

// bool value array
$bool_values = array (1 => true, 2 => false, 3 => TRUE, 4 => FALSE);

echo "\n-- Testing asort() by supplying bool value array, 'flag' value is defualt --\n";
$temp_array = $bool_values;
var_dump(asort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing asort() by supplying bool value array, 'flag' value is SORT_REGULAR --\n";
$temp_array = $bool_values;
var_dump(asort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "\n-- Testing asort() by supplying bool value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $bool_values;
var_dump(asort($temp_array, SORT_NUMERIC) );
var_dump($temp_array);

echo "\n-- Testing asort() by supplying bool value array, 'flag' value is SORT_STRING --\n";
$temp_array = $bool_values;
var_dump(asort($temp_array, SORT_STRING) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECT--
*** Testing asort() : usage variations ***

-- Testing asort() by supplying bool value array, 'flag' value is defualt --
bool(true)
array(4) {
  [2]=>
  bool(false)
  [4]=>
  bool(false)
  [1]=>
  bool(true)
  [3]=>
  bool(true)
}

-- Testing asort() by supplying bool value array, 'flag' value is SORT_REGULAR --
bool(true)
array(4) {
  [2]=>
  bool(false)
  [4]=>
  bool(false)
  [1]=>
  bool(true)
  [3]=>
  bool(true)
}

-- Testing asort() by supplying bool value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(4) {
  [2]=>
  bool(false)
  [4]=>
  bool(false)
  [1]=>
  bool(true)
  [3]=>
  bool(true)
}

-- Testing asort() by supplying bool value array, 'flag' value is SORT_STRING --
bool(true)
array(4) {
  [2]=>
  bool(false)
  [4]=>
  bool(false)
  [1]=>
  bool(true)
  [3]=>
  bool(true)
}
Done
