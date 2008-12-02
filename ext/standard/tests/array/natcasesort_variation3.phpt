--TEST--
Test natcasesort() function : usage variations - different numeric types
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : bool natcasesort(array &$array_arg)
 * Description: Sort an array using case-insensitive natural sort
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays of numeric data to test how natcasesort re-orders the array
 */

echo "*** Testing natcasesort() : usage variation ***\n";

$inputs = array (

  // negative/positive integers array
  array(11, -11, 21, -21, 31, -31, 0, 41, -41),

  // float value array
  array(10.5, -10.5, 10.5e2, 10.6E-2, .5, .01, -.1),

  // mixed value array
  array(.0001, .0021, -.01, -1, 0, .09, 2, -.9, 10.6E-2, -10.6E-2, 33),
 
  // array values contains minimum and maximum ranges
  array(2147483647, 2147483648, -2147483647, -2147483648, -0, 0, -2147483649)
);

$iterator = 1;
foreach ($inputs as $array_arg) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(natcasesort($array_arg));
	var_dump($array_arg);
}

echo "Done";
?>

--EXPECTF--
*** Testing natcasesort() : usage variation ***

-- Iteration 1 --
bool(true)
array(9) {
  [1]=>
  int(-11)
  [3]=>
  int(-21)
  [5]=>
  int(-31)
  [8]=>
  int(-41)
  [6]=>
  int(0)
  [0]=>
  int(11)
  [2]=>
  int(21)
  [4]=>
  int(31)
  [7]=>
  int(41)
}

-- Iteration 1 --
bool(true)
array(7) {
  [6]=>
  float(-0.1)
  [1]=>
  float(-10.5)
  [5]=>
  float(0.01)
  [4]=>
  float(0.5)
  [3]=>
  float(0.106)
  [0]=>
  float(10.5)
  [2]=>
  float(1050)
}

-- Iteration 1 --
bool(true)
array(11) {
  [2]=>
  float(-0.01)
  [7]=>
  float(-0.9)
  [9]=>
  float(-0.106)
  [3]=>
  int(-1)
  [4]=>
  int(0)
  [0]=>
  float(0.0001)
  [1]=>
  float(0.0021)
  [5]=>
  float(0.09)
  [8]=>
  float(0.106)
  [6]=>
  int(2)
  [10]=>
  int(33)
}

-- Iteration 1 --
bool(true)
array(7) {
  [2]=>
  int(-2147483647)
  [3]=>
  float(-2147483648)
  [6]=>
  float(-2147483649)
  [5]=>
  int(0)
  [4]=>
  int(0)
  [0]=>
  int(2147483647)
  [1]=>
  float(2147483648)
}
Done