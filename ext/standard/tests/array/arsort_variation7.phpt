--TEST--
Test arsort() function : usage variations - sort bool values 
--FILE--
<?php
/* Prototype  : bool arsort ( array &$array [, int $sort_flags] )
 * Description: This function arsorts an array. 
                Elements will be arranged from highest to lowest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * testing arsort() by providing bool value array for $array argument with following flag values.
 * flag value as defualt
 * SORT_REGULAR - compare items normally
*/

echo "*** Testing arsort() : usage variations ***\n";

// bool value array
$bool_values = array (1 => true, 2 => false, 3 => TRUE, 4 => FALSE);

echo "\n-- Testing arsort() by supplying bool value array, 'flag' value is defualt --\n";
$temp_array = $bool_values;
var_dump(arsort($temp_array) );
var_dump($temp_array);

echo "\n-- Testing arsort() by supplying bool value array, 'flag' value is SORT_REGULAR --\n";
$temp_array = $bool_values;
var_dump(arsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "\n-- Testing arsort() by supplying bool value array, 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $bool_values;
var_dump(arsort($temp_array, SORT_NUMERIC) );
var_dump($temp_array);

echo "\n-- Testing arsort() by supplying bool value array, 'flag' value is SORT_STRING --\n";
$temp_array = $bool_values;
var_dump(arsort($temp_array, SORT_STRING) );
var_dump($temp_array);

echo "Done\n";
?>
--EXPECTF--
*** Testing arsort() : usage variations ***

-- Testing arsort() by supplying bool value array, 'flag' value is defualt --
bool(true)
array(4) {
  [1]=>
  bool(true)
  [3]=>
  bool(true)
  [2]=>
  bool(false)
  [4]=>
  bool(false)
}

-- Testing arsort() by supplying bool value array, 'flag' value is SORT_REGULAR --
bool(true)
array(4) {
  [1]=>
  bool(true)
  [3]=>
  bool(true)
  [2]=>
  bool(false)
  [4]=>
  bool(false)
}

-- Testing arsort() by supplying bool value array, 'flag' value is SORT_NUMERIC  --
bool(true)
array(4) {
  [1]=>
  bool(true)
  [3]=>
  bool(true)
  [2]=>
  bool(false)
  [4]=>
  bool(false)
}

-- Testing arsort() by supplying bool value array, 'flag' value is SORT_STRING --
bool(true)
array(4) {
  [1]=>
  bool(true)
  [3]=>
  bool(true)
  [2]=>
  bool(false)
  [4]=>
  bool(false)
}
Done
