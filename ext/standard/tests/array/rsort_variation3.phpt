--TEST--
Test rsort() function : usage variations - numeric values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : bool rsort(array &$array_arg [, int $sort_flags])
 * Description: Sort an array in reverse order
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays containing different numeric data to rsort() to test behaviour
 */

echo "*** Testing rsort() : variation ***\n";

// group of various arrays

$various_arrays = array (
// negative/positive integers array
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

// loop through to test rsort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With Defualt sort flag -\n";
  $temp_array = $array;
  var_dump(rsort($temp_array) );
  var_dump($temp_array);

  // loop through $flag_value array and setting all possible flag values
  foreach($flag_value as $key => $flag){
    echo "- Sort flag = $key -\n";
    $temp_array = $array;
    var_dump(rsort($temp_array, $flag) );
    var_dump($temp_array);
  }
  $count++;
}

echo "Done";
?>
--EXPECT--
*** Testing rsort() : variation ***

-- Iteration 1 --
- With Defualt sort flag -
bool(true)
array(9) {
  [0]=>
  int(41)
  [1]=>
  int(31)
  [2]=>
  int(21)
  [3]=>
  int(11)
  [4]=>
  int(0)
  [5]=>
  int(-11)
  [6]=>
  int(-21)
  [7]=>
  int(-31)
  [8]=>
  int(-41)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(9) {
  [0]=>
  int(41)
  [1]=>
  int(31)
  [2]=>
  int(21)
  [3]=>
  int(11)
  [4]=>
  int(0)
  [5]=>
  int(-11)
  [6]=>
  int(-21)
  [7]=>
  int(-31)
  [8]=>
  int(-41)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(9) {
  [0]=>
  int(41)
  [1]=>
  int(31)
  [2]=>
  int(21)
  [3]=>
  int(11)
  [4]=>
  int(0)
  [5]=>
  int(-11)
  [6]=>
  int(-21)
  [7]=>
  int(-31)
  [8]=>
  int(-41)
}

-- Iteration 2 --
- With Defualt sort flag -
bool(true)
array(7) {
  [0]=>
  float(1050)
  [1]=>
  float(10.5)
  [2]=>
  float(0.5)
  [3]=>
  float(0.106)
  [4]=>
  float(0.01)
  [5]=>
  float(-0.1)
  [6]=>
  float(-10.5)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(7) {
  [0]=>
  float(1050)
  [1]=>
  float(10.5)
  [2]=>
  float(0.5)
  [3]=>
  float(0.106)
  [4]=>
  float(0.01)
  [5]=>
  float(-0.1)
  [6]=>
  float(-10.5)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(7) {
  [0]=>
  float(1050)
  [1]=>
  float(10.5)
  [2]=>
  float(0.5)
  [3]=>
  float(0.106)
  [4]=>
  float(0.01)
  [5]=>
  float(-0.1)
  [6]=>
  float(-10.5)
}

-- Iteration 3 --
- With Defualt sort flag -
bool(true)
array(11) {
  [0]=>
  int(33)
  [1]=>
  int(2)
  [2]=>
  float(0.106)
  [3]=>
  float(0.09)
  [4]=>
  float(0.0021)
  [5]=>
  float(0.0001)
  [6]=>
  int(0)
  [7]=>
  float(-0.01)
  [8]=>
  float(-0.106)
  [9]=>
  float(-0.9)
  [10]=>
  int(-1)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(11) {
  [0]=>
  int(33)
  [1]=>
  int(2)
  [2]=>
  float(0.106)
  [3]=>
  float(0.09)
  [4]=>
  float(0.0021)
  [5]=>
  float(0.0001)
  [6]=>
  int(0)
  [7]=>
  float(-0.01)
  [8]=>
  float(-0.106)
  [9]=>
  float(-0.9)
  [10]=>
  int(-1)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(11) {
  [0]=>
  int(33)
  [1]=>
  int(2)
  [2]=>
  float(0.106)
  [3]=>
  float(0.09)
  [4]=>
  float(0.0021)
  [5]=>
  float(0.0001)
  [6]=>
  int(0)
  [7]=>
  float(-0.01)
  [8]=>
  float(-0.106)
  [9]=>
  float(-0.9)
  [10]=>
  int(-1)
}

-- Iteration 4 --
- With Defualt sort flag -
bool(true)
array(7) {
  [0]=>
  float(2147483648)
  [1]=>
  int(2147483647)
  [2]=>
  int(0)
  [3]=>
  int(0)
  [4]=>
  int(-2147483647)
  [5]=>
  float(-2147483648)
  [6]=>
  float(-2147483649)
}
- Sort flag = SORT_REGULAR -
bool(true)
array(7) {
  [0]=>
  float(2147483648)
  [1]=>
  int(2147483647)
  [2]=>
  int(0)
  [3]=>
  int(0)
  [4]=>
  int(-2147483647)
  [5]=>
  float(-2147483648)
  [6]=>
  float(-2147483649)
}
- Sort flag = SORT_NUMERIC -
bool(true)
array(7) {
  [0]=>
  float(2147483648)
  [1]=>
  int(2147483647)
  [2]=>
  int(0)
  [3]=>
  int(0)
  [4]=>
  int(-2147483647)
  [5]=>
  float(-2147483648)
  [6]=>
  float(-2147483649)
}
Done
