--TEST--
Test uasort() function : usage variations - sort different numeric values
--FILE--
<?php
/*
* sorting different types of numeric arrays containing data of following type:
*  integer, octal, hexadecimal & float
*/

// comparison function
function cmp_function($value1, $value2)
{
  if($value1 == $value2) {
    return 0;
  }
  else if($value1 > $value2) {
    return 1;
  }
  else {
    return -1;
  }
}

echo "*** Testing uasort() : different numeric arrays as 'array_arg' ***\n";

// Int array
$int_values = array(0 => 3, 1 => 2, 3 => 100, 4 => 150, 5 => 25, 6 => 350, 7 => 0, 8 => -3, 9 => -1200);
echo "-- Sorting Integer array --\n";
var_dump( uasort($int_values, 'cmp_function') );  // expecting: bool(true)
var_dump($int_values);

// Octal array
$octal_values = array(0 => 056, 1 => 023, 2 => 00, 3 => 015, 4 => -045, 5 => 01, 6 => -07);
echo "-- Sorting Octal array --\n";
var_dump( uasort($octal_values, 'cmp_function') );  // expecting: bool(true)
var_dump($octal_values);

// Hexadecimal array
$hex_values = array(0 => 0xAE, 1 => 0x2B, 2 => 0X10, 3 => -0xCF, 4 => 0X12, 5 => -0XF2);
echo "-- Sorting Hex array --\n";
var_dump( uasort($hex_values, 'cmp_function') );  // expecting: bool(true)
var_dump($hex_values);

// Float array
$float_values = array( 0 => 10.2, 1 => 2.4, 2 => -3.4, 3 => 0, 4 => 0.5, 5 => 7.3e3, 6 => -9.34E-2);
echo "-- Sorting Float array --\n";
var_dump( uasort($float_values, 'cmp_function') );  // expecting: bool(true)
var_dump($float_values);

// empty array
$empty_array = array();
echo "-- Sorting empty array --\n";
var_dump( uasort($empty_array, 'cmp_function') );  // expecting: bool(true)
var_dump($empty_array);

echo "Done"
?>
--EXPECT--
*** Testing uasort() : different numeric arrays as 'array_arg' ***
-- Sorting Integer array --
bool(true)
array(9) {
  [9]=>
  int(-1200)
  [8]=>
  int(-3)
  [7]=>
  int(0)
  [1]=>
  int(2)
  [0]=>
  int(3)
  [5]=>
  int(25)
  [3]=>
  int(100)
  [4]=>
  int(150)
  [6]=>
  int(350)
}
-- Sorting Octal array --
bool(true)
array(7) {
  [4]=>
  int(-37)
  [6]=>
  int(-7)
  [2]=>
  int(0)
  [5]=>
  int(1)
  [3]=>
  int(13)
  [1]=>
  int(19)
  [0]=>
  int(46)
}
-- Sorting Hex array --
bool(true)
array(6) {
  [5]=>
  int(-242)
  [3]=>
  int(-207)
  [2]=>
  int(16)
  [4]=>
  int(18)
  [1]=>
  int(43)
  [0]=>
  int(174)
}
-- Sorting Float array --
bool(true)
array(7) {
  [2]=>
  float(-3.4)
  [6]=>
  float(-0.0934)
  [3]=>
  int(0)
  [4]=>
  float(0.5)
  [1]=>
  float(2.4)
  [0]=>
  float(10.2)
  [5]=>
  float(7300)
}
-- Sorting empty array --
bool(true)
array(0) {
}
Done
