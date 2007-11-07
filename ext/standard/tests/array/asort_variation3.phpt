--TEST--
Test asort() function : usage variations - sort integer/float values
--FILE--
<?php
/* Prototype  : bool asort ( array &$array [, int $sort_flags] )
 * Description: Sort an array and maintain index association 
                Elements will be arranged from lowest to highest when this function has completed.
 * Source code: ext/standard/array.c
*/

/*
 * Testing asort() by providing different integer/float value arrays for $array argument with following values
 * 1. flag value as defualt
 * 2. SORT_REGULAR - compare items normally
 * 3. SORT_NUMERIC - compare items numerically
*/

echo "*** Testing asort() : usage variations ***\n";

// group of various arrays with indices
$various_arrays = array(
  // negative/posative integer array
  array(1 => 11, 2 => -11, 3 => 21, 4 => -21, 5 => 31, 6 => -31, 7 => 0, 8 => 41, 10 =>-41),

  // float value array
  array(1 => 10.5, 2 => -10.5, 3 => 10.5e2, 4 => 10.6E-2, 5 => .5, 6 => .0001, 7 => -.1),

  // mixed value array
  array(1 => .0001, 2 => .0021, 3 => -.01, 4 => -1, 5 => 0, 6 => .09, 7 => 2, 8 => -.9, 9 => 10.6E-2, 10 => -10.6E-2, 11 => 33),
 
  // array values contains minimum and maximum ranges
  array(1 => 2147483647, 2 => 2147483648, 3 => -2147483647, 4 => -2147483648, 5 => -0, 6 => 0, 7 => -2147483649)
);

// set of possible flag values
$flag_value = array("SORT_REGULAR" => SORT_REGULAR, "SORT_NUMERIC" => SORT_NUMERIC);

$count = 1;
echo "\n-- Testing asort() by supplying various integer/float arrays --\n";

// loop through to test asort() with different arrays
foreach ($various_arrays as $array) {
  echo "\n-- Iteration $count --\n";

  echo "- With default sort_flag -\n"; 
  $temp_array = $array; 
  var_dump(asort($temp_array) );
  var_dump($temp_array);

  // loop through $flag_value array and setting all possible flag values
  foreach($flag_value as $key => $flag){
    echo "- Sort_flag = $key -\n";
    $temp_array = $array; 
    var_dump(asort($temp_array, $flag) );
    var_dump($temp_array);
  }  
  $count++;
} 

echo "Done\n";
?>
--EXPECTF--
*** Testing asort() : usage variations ***

-- Testing asort() by supplying various integer/float arrays --

-- Iteration 1 --
- With default sort_flag -
bool(true)
array(9) {
  [10]=>
  int(-41)
  [6]=>
  int(-31)
  [4]=>
  int(-21)
  [2]=>
  int(-11)
  [7]=>
  int(0)
  [1]=>
  int(11)
  [3]=>
  int(21)
  [5]=>
  int(31)
  [8]=>
  int(41)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(9) {
  [10]=>
  int(-41)
  [6]=>
  int(-31)
  [4]=>
  int(-21)
  [2]=>
  int(-11)
  [7]=>
  int(0)
  [1]=>
  int(11)
  [3]=>
  int(21)
  [5]=>
  int(31)
  [8]=>
  int(41)
}
- Sort_flag = SORT_NUMERIC -
bool(true)
array(9) {
  [10]=>
  int(-41)
  [6]=>
  int(-31)
  [4]=>
  int(-21)
  [2]=>
  int(-11)
  [7]=>
  int(0)
  [1]=>
  int(11)
  [3]=>
  int(21)
  [5]=>
  int(31)
  [8]=>
  int(41)
}

-- Iteration 2 --
- With default sort_flag -
bool(true)
array(7) {
  [2]=>
  float(-10.5)
  [7]=>
  float(-0.1)
  [6]=>
  float(0.0001)
  [4]=>
  float(0.106)
  [5]=>
  float(0.5)
  [1]=>
  float(10.5)
  [3]=>
  float(1050)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(7) {
  [2]=>
  float(-10.5)
  [7]=>
  float(-0.1)
  [6]=>
  float(0.0001)
  [4]=>
  float(0.106)
  [5]=>
  float(0.5)
  [1]=>
  float(10.5)
  [3]=>
  float(1050)
}
- Sort_flag = SORT_NUMERIC -
bool(true)
array(7) {
  [2]=>
  float(-10.5)
  [7]=>
  float(-0.1)
  [6]=>
  float(0.0001)
  [4]=>
  float(0.106)
  [5]=>
  float(0.5)
  [1]=>
  float(10.5)
  [3]=>
  float(1050)
}

-- Iteration 3 --
- With default sort_flag -
bool(true)
array(11) {
  [4]=>
  int(-1)
  [8]=>
  float(-0.9)
  [10]=>
  float(-0.106)
  [3]=>
  float(-0.01)
  [5]=>
  int(0)
  [1]=>
  float(0.0001)
  [2]=>
  float(0.0021)
  [6]=>
  float(0.09)
  [9]=>
  float(0.106)
  [7]=>
  int(2)
  [11]=>
  int(33)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(11) {
  [4]=>
  int(-1)
  [8]=>
  float(-0.9)
  [10]=>
  float(-0.106)
  [3]=>
  float(-0.01)
  [5]=>
  int(0)
  [1]=>
  float(0.0001)
  [2]=>
  float(0.0021)
  [6]=>
  float(0.09)
  [9]=>
  float(0.106)
  [7]=>
  int(2)
  [11]=>
  int(33)
}
- Sort_flag = SORT_NUMERIC -
bool(true)
array(11) {
  [4]=>
  int(-1)
  [8]=>
  float(-0.9)
  [10]=>
  float(-0.106)
  [3]=>
  float(-0.01)
  [5]=>
  int(0)
  [1]=>
  float(0.0001)
  [2]=>
  float(0.0021)
  [6]=>
  float(0.09)
  [9]=>
  float(0.106)
  [7]=>
  int(2)
  [11]=>
  int(33)
}

-- Iteration 4 --
- With default sort_flag -
bool(true)
array(7) {
  [7]=>
  %s(-2147483649)
  [4]=>
  %s(-2147483648)
  [3]=>
  int(-2147483647)
  [6]=>
  int(0)
  [5]=>
  int(0)
  [1]=>
  int(2147483647)
  [2]=>
  %s(2147483648)
}
- Sort_flag = SORT_REGULAR -
bool(true)
array(7) {
  [7]=>
  %s(-2147483649)
  [4]=>
  %s(-2147483648)
  [3]=>
  int(-2147483647)
  [6]=>
  int(0)
  [5]=>
  int(0)
  [1]=>
  int(2147483647)
  [2]=>
  %s(2147483648)
}
- Sort_flag = SORT_NUMERIC -
bool(true)
array(7) {
  [7]=>
  %s(-2147483649)
  [4]=>
  %s(-2147483648)
  [3]=>
  int(-2147483647)
  [6]=>
  int(0)
  [5]=>
  int(0)
  [1]=>
  int(2147483647)
  [2]=>
  %s(2147483648)
}
Done
