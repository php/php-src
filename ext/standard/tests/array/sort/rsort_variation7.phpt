--TEST--
Test rsort() function : usage variations - boolean values
--FILE--
<?php
/*
 * Pass rsort() arrays of boolean values to test behaviour
 */

echo "*** Testing rsort() : variation ***\n";

// bool value array
$bool_values = array (true, false, TRUE, FALSE);

echo "\n-- 'flag' value is default --\n";
$temp_array = $bool_values;
var_dump(rsort($temp_array) );
var_dump($temp_array);

echo "\n-- 'flag' value is SORT_REGULAR --\n";
$temp_array = $bool_values;
var_dump(rsort($temp_array, SORT_REGULAR) );
var_dump($temp_array);

echo "\n-- 'flag' value is SORT_NUMERIC  --\n";
$temp_array = $bool_values;
var_dump(rsort($temp_array, SORT_NUMERIC) );
var_dump($temp_array);

echo "\n-- 'flag' value is SORT_STRING --\n";
$temp_array = $bool_values;
var_dump(rsort($temp_array, SORT_STRING) );
var_dump($temp_array);

echo "Done";
?>
--EXPECT--
*** Testing rsort() : variation ***

-- 'flag' value is default --
bool(true)
array(4) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
  [2]=>
  bool(false)
  [3]=>
  bool(false)
}

-- 'flag' value is SORT_REGULAR --
bool(true)
array(4) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
  [2]=>
  bool(false)
  [3]=>
  bool(false)
}

-- 'flag' value is SORT_NUMERIC  --
bool(true)
array(4) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
  [2]=>
  bool(false)
  [3]=>
  bool(false)
}

-- 'flag' value is SORT_STRING --
bool(true)
array(4) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
  [2]=>
  bool(false)
  [3]=>
  bool(false)
}
Done
