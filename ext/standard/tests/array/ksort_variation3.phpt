--TEST--
Test ksort() function : usage variations - sort integer/float values
--FILE--
<?php
/*
 * Testing ksort() by providing array of integer/float/mixed values for $array argument
 * with following flag  values:
 *  1. flag value as default
 *  2. SORT_REGULAR - compare items normally
 *  3. SORT_NUMERIC - compare items numerically
*/

echo "*** Testing ksort() : usage variations ***\n";

// diff. associative arrays to sort
$various_arrays = array(
  // negative/posative integer key value array
  array(1 => 11, -2 => -11, 3 => 21, -4 => -21, 5 => 31, -6 => -31, 7 => 0, 8 => 41, -10 =>-41),

  // float key values
  array(1.0 => 10.5, 0.2 => -10.5, 3.1 => 10.5e2, 4 => 10.6E-2, .5 => .5, 6 => .0001, -7 => -.1),

  // mixed value array with different types of keys
  array(1 => .0001, 2 => .0021, -3 => -.01, 4 => -1, 5 => 0, 6 => .09, 7 => 2, -8 => -.9,
        9 => 10.6E-2, -10 => -10.6E-2, 11 => 33)
);

// set of possible flag values
$flags = array("SORT_REGULAR" => SORT_REGULAR, "SORT_NUMERIC" => SORT_NUMERIC);

$count = 1;
echo "\n-- Testing ksort() by supplying various integer/float arrays --\n";

// loop through to test ksort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort flag -\n";
  $temp_array = $array;
  var_dump(ksort($temp_array) );
  var_dump($temp_array);

  // loop through $flags array and call ksort() with all possible sort flag values
  foreach($flags as $key => $flag){
    echo "- Sort flag = $key -\n";
    $temp_array = $array;
    var_dump(ksort($temp_array, $flag) );
    var_dump($temp_array);
  }
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing ksort() : usage variations ***

-- Testing ksort() by supplying various integer/float arrays --

-- Iteration 1 --
- With default sort flag -
bool(true)
array(9) {
  [-10]=>
  int(-41)
  [-6]=>
  int(-31)
  [-4]=>
  int(-21)
  [-2]=>
  int(-11)
  [1]=>
  int(11)
  [3]=>
  int(21)
  [5]=>
  int(31)
  [7]=>
  int(0)
  [8]=>
  int(41)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(9) {
  [-10]=>
  int(-41)
  [-6]=>
  int(-31)
  [-4]=>
  int(-21)
  [-2]=>
  int(-11)
  [1]=>
  int(11)
  [3]=>
  int(21)
  [5]=>
  int(31)
  [7]=>
  int(0)
  [8]=>
  int(41)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(9) {
  [-10]=>
  int(-41)
  [-6]=>
  int(-31)
  [-4]=>
  int(-21)
  [-2]=>
  int(-11)
  [1]=>
  int(11)
  [3]=>
  int(21)
  [5]=>
  int(31)
  [7]=>
  int(0)
  [8]=>
  int(41)
}

-- Iteration 2 --
- With default sort flag -
bool(true)
array(6) {
  [-7]=>
  float(-0.1)
  [0]=>
  float(0.5)
  [1]=>
  float(10.5)
  [3]=>
  float(1050)
  [4]=>
  float(0.106)
  [6]=>
  float(0.0001)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(6) {
  [-7]=>
  float(-0.1)
  [0]=>
  float(0.5)
  [1]=>
  float(10.5)
  [3]=>
  float(1050)
  [4]=>
  float(0.106)
  [6]=>
  float(0.0001)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(6) {
  [-7]=>
  float(-0.1)
  [0]=>
  float(0.5)
  [1]=>
  float(10.5)
  [3]=>
  float(1050)
  [4]=>
  float(0.106)
  [6]=>
  float(0.0001)
}

-- Iteration 3 --
- With default sort flag -
bool(true)
array(11) {
  [-10]=>
  float(-0.106)
  [-8]=>
  float(-0.9)
  [-3]=>
  float(-0.01)
  [1]=>
  float(0.0001)
  [2]=>
  float(0.0021)
  [4]=>
  int(-1)
  [5]=>
  int(0)
  [6]=>
  float(0.09)
  [7]=>
  int(2)
  [9]=>
  float(0.106)
  [11]=>
  int(33)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  [-10]=>
  float(-0.106)
  [-8]=>
  float(-0.9)
  [-3]=>
  float(-0.01)
  [1]=>
  float(0.0001)
  [2]=>
  float(0.0021)
  [4]=>
  int(-1)
  [5]=>
  int(0)
  [6]=>
  float(0.09)
  [7]=>
  int(2)
  [9]=>
  float(0.106)
  [11]=>
  int(33)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(11) {
  [-10]=>
  float(-0.106)
  [-8]=>
  float(-0.9)
  [-3]=>
  float(-0.01)
  [1]=>
  float(0.0001)
  [2]=>
  float(0.0021)
  [4]=>
  int(-1)
  [5]=>
  int(0)
  [6]=>
  float(0.09)
  [7]=>
  int(2)
  [9]=>
  float(0.106)
  [11]=>
  int(33)
}
Done
