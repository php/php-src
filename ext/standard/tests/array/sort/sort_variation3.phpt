--TEST--
Test sort() function : usage variations - sort integer/float values
--FILE--
<?php
/*
 * Testing sort() by providing different integer/float value arrays for $array argument
 * with following flag values
 * 1. flag  value as default
 * 2. SORT_REGULAR - compare items normally
 * 3. SORT_NUMERIC - compare items numerically
 * 4. SORT_STRING - compare items as strings
*/

echo "*** Testing sort() : usage variations ***\n";

// group of various arrays
$various_arrays = array (
  // negative/posative integers array
  array(11, -11, 21, -21, 31, -31, 0, 41, -41),

  // float value array
  array(10.5, -10.5, 10.5e2, 10.6E-2, .5, .01, -.1),

  // mixed value array
  array(.0001, .0021, -.01, -1, 0, .09, 2, -.9, 10.6E-2, -10.6E-2, 33),

  // array values contains minimum and maximum ranges
  array(2147483647, 2147483648, -2147483647, -2147483648, -0, 0, -2147483649)
);

// set of possible flag values
$flag_value = array("SORT_REGULAR" => SORT_REGULAR, "SORT_NUMERIC" => SORT_NUMERIC);

$count = 1;
echo "\n-- Testing sort() by supplying various integer/float arrays --\n";

// loop through to test sort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With Default sort flag -\n";
  $temp_array = $array;
  var_dump(sort($temp_array) );
  var_dump($temp_array);

  // loop through $flag_value array and setting all possible flag values
  foreach($flag_value as $key => $flag){
    echo "- Sort flag = $key -\n";
    $temp_array = $array;
    var_dump(sort($temp_array, $flag) );
    var_dump($temp_array);
  }
  $count++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing sort() : usage variations ***

-- Testing sort() by supplying various integer/float arrays --

-- Iteration 1 --
- With Default sort flag -
bool(true)
array(9) {
  [0]=>
  int(-41)
  [1]=>
  int(-31)
  [2]=>
  int(-21)
  [3]=>
  int(-11)
  [4]=>
  int(0)
  [5]=>
  int(11)
  [6]=>
  int(21)
  [7]=>
  int(31)
  [8]=>
  int(41)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(9) {
  [0]=>
  int(-41)
  [1]=>
  int(-31)
  [2]=>
  int(-21)
  [3]=>
  int(-11)
  [4]=>
  int(0)
  [5]=>
  int(11)
  [6]=>
  int(21)
  [7]=>
  int(31)
  [8]=>
  int(41)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(9) {
  [0]=>
  int(-41)
  [1]=>
  int(-31)
  [2]=>
  int(-21)
  [3]=>
  int(-11)
  [4]=>
  int(0)
  [5]=>
  int(11)
  [6]=>
  int(21)
  [7]=>
  int(31)
  [8]=>
  int(41)
}

-- Iteration 2 --
- With Default sort flag -
bool(true)
array(7) {
  [0]=>
  float(-10.5)
  [1]=>
  float(-0.1)
  [2]=>
  float(0.01)
  [3]=>
  float(0.106)
  [4]=>
  float(0.5)
  [5]=>
  float(10.5)
  [6]=>
  float(1050)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(7) {
  [0]=>
  float(-10.5)
  [1]=>
  float(-0.1)
  [2]=>
  float(0.01)
  [3]=>
  float(0.106)
  [4]=>
  float(0.5)
  [5]=>
  float(10.5)
  [6]=>
  float(1050)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(7) {
  [0]=>
  float(-10.5)
  [1]=>
  float(-0.1)
  [2]=>
  float(0.01)
  [3]=>
  float(0.106)
  [4]=>
  float(0.5)
  [5]=>
  float(10.5)
  [6]=>
  float(1050)
}

-- Iteration 3 --
- With Default sort flag -
bool(true)
array(11) {
  [0]=>
  int(-1)
  [1]=>
  float(-0.9)
  [2]=>
  float(-0.106)
  [3]=>
  float(-0.01)
  [4]=>
  int(0)
  [5]=>
  float(0.0001)
  [6]=>
  float(0.0021)
  [7]=>
  float(0.09)
  [8]=>
  float(0.106)
  [9]=>
  int(2)
  [10]=>
  int(33)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  [0]=>
  int(-1)
  [1]=>
  float(-0.9)
  [2]=>
  float(-0.106)
  [3]=>
  float(-0.01)
  [4]=>
  int(0)
  [5]=>
  float(0.0001)
  [6]=>
  float(0.0021)
  [7]=>
  float(0.09)
  [8]=>
  float(0.106)
  [9]=>
  int(2)
  [10]=>
  int(33)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(11) {
  [0]=>
  int(-1)
  [1]=>
  float(-0.9)
  [2]=>
  float(-0.106)
  [3]=>
  float(-0.01)
  [4]=>
  int(0)
  [5]=>
  float(0.0001)
  [6]=>
  float(0.0021)
  [7]=>
  float(0.09)
  [8]=>
  float(0.106)
  [9]=>
  int(2)
  [10]=>
  int(33)
}

-- Iteration 4 --
- With Default sort flag -
bool(true)
array(7) {
  [0]=>
  %s(-2147483649)
  [1]=>
  %s(-2147483648)
  [2]=>
  int(-2147483647)
  [3]=>
  int(0)
  [4]=>
  int(0)
  [5]=>
  int(2147483647)
  [6]=>
  %s(2147483648)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(7) {
  [0]=>
  %s(-2147483649)
  [1]=>
  %s(-2147483648)
  [2]=>
  int(-2147483647)
  [3]=>
  int(0)
  [4]=>
  int(0)
  [5]=>
  int(2147483647)
  [6]=>
  %s(2147483648)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(7) {
  [0]=>
  %s(-2147483649)
  [1]=>
  %s(-2147483648)
  [2]=>
  int(-2147483647)
  [3]=>
  int(0)
  [4]=>
  int(0)
  [5]=>
  int(2147483647)
  [6]=>
  %s(2147483648)
}
Done
