--TEST--
Test krsort() function : usage variations - sort integer/float values
--FILE--
<?php
/*
 * Testing krsort() by providing array of integer/float/mixed values for $array argument
 * with following flag values:
 *  1.flag value as default
 *  2.SORT_REGULAR - compare items normally
 *  3.SORT_NUMERIC - compare items numerically
*/

echo "*** Testing krsort() : usage variations ***\n";

// diff. associative arrays to sort
$various_arrays = array(
  // negative/positive integer key value array
  array(1 => 11, -2 => -11, 3 => 21, -4 => -21, 5 => 31, -6 => -31, 7 => 0, 8 => 41, -10 =>-41),

  // mixed value array with different types of keys
  array(1 => .0001, 2 => .0021, -3 => -.01, 4 => -1, 5 => 0, 6 => .09, 7 => 2, -8 => -.9, 9 => 10.6E-2,
        -10 => -10.6E-2, 11 => 33)
);

// set of possible flag values
$flags = array("SORT_REGULAR" => SORT_REGULAR, "SORT_NUMERIC" => SORT_NUMERIC);

$count = 1;
echo "\n-- Testing krsort() by supplying various integer/float arrays --\n";

// loop through to test krsort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort flag -\n";
  $temp_array = $array;
  var_dump(krsort($temp_array) );
  var_dump($temp_array);

  // loop through $flags array and call krsort() with all possible sort flag values
  foreach($flags as $key => $flag){
    echo "- Sort flag = $key -\n";
    $temp_array = $array;
    var_dump(krsort($temp_array, $flag) );
    var_dump($temp_array);
  }
  $count++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing krsort() : usage variations ***

-- Testing krsort() by supplying various integer/float arrays --

-- Iteration 1 --
- With default sort flag -
bool(true)
array(9) {
  [8]=>
  int(41)
  [7]=>
  int(0)
  [5]=>
  int(31)
  [3]=>
  int(21)
  [1]=>
  int(11)
  [-2]=>
  int(-11)
  [-4]=>
  int(-21)
  [-6]=>
  int(-31)
  [-10]=>
  int(-41)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(9) {
  [8]=>
  int(41)
  [7]=>
  int(0)
  [5]=>
  int(31)
  [3]=>
  int(21)
  [1]=>
  int(11)
  [-2]=>
  int(-11)
  [-4]=>
  int(-21)
  [-6]=>
  int(-31)
  [-10]=>
  int(-41)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(9) {
  [8]=>
  int(41)
  [7]=>
  int(0)
  [5]=>
  int(31)
  [3]=>
  int(21)
  [1]=>
  int(11)
  [-2]=>
  int(-11)
  [-4]=>
  int(-21)
  [-6]=>
  int(-31)
  [-10]=>
  int(-41)
}

-- Iteration 2 --
- With default sort flag -
bool(true)
array(11) {
  [11]=>
  int(33)
  [9]=>
  float(0.106)
  [7]=>
  int(2)
  [6]=>
  float(0.09)
  [5]=>
  int(0)
  [4]=>
  int(-1)
  [2]=>
  float(0.0021)
  [1]=>
  float(0.0001)
  [-3]=>
  float(-0.01)
  [-8]=>
  float(-0.9)
  [-10]=>
  float(-0.106)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  [11]=>
  int(33)
  [9]=>
  float(0.106)
  [7]=>
  int(2)
  [6]=>
  float(0.09)
  [5]=>
  int(0)
  [4]=>
  int(-1)
  [2]=>
  float(0.0021)
  [1]=>
  float(0.0001)
  [-3]=>
  float(-0.01)
  [-8]=>
  float(-0.9)
  [-10]=>
  float(-0.106)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(11) {
  [11]=>
  int(33)
  [9]=>
  float(0.106)
  [7]=>
  int(2)
  [6]=>
  float(0.09)
  [5]=>
  int(0)
  [4]=>
  int(-1)
  [2]=>
  float(0.0021)
  [1]=>
  float(0.0001)
  [-3]=>
  float(-0.01)
  [-8]=>
  float(-0.9)
  [-10]=>
  float(-0.106)
}
Done
