--TEST--
Test usort() function : usage variations - numeric data
--FILE--
<?php
/* Prototype  : bool usort(array $array_arg, string $cmp_function)
 * Description: Sort an array by values using a user-defined comparison function
 * Source code: ext/standard/array.c
 */

/*
 * Pass arrays of numeric data to usort() to test how it is re-ordered
 */

echo "*** Testing usort() : usage variation ***\n";

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

// Int array
$int_values = array(0 => 3,   1 => 2,   3 => 100,
                    4 => 150, 5 => 25,  6 => 350,
                    7 => 0,   8 => -3,  9 => -1200);

echo "\n-- Sorting Integer array --\n";
var_dump( usort($int_values, 'cmp_function') );
var_dump($int_values);

// Octal array
$octal_values = array(0 => 056, 1 => 023,  2 => 00,
                      3 => 015, 4 => -045, 5 => 01,  6 => -07);

echo "\n-- Sorting Octal array --\n";
var_dump( usort($octal_values, 'cmp_function') );
var_dump($octal_values);

// Hexadecimal array
$hex_values = array(0 => 0xAE,  1 => 0x2B, 2 => 0X10,
                    3 => -0xCF, 4 => 0X12, 5 => -0XF2);

echo "\n-- Sorting Hex array --\n";
var_dump( usort($hex_values, 'cmp_function') );
var_dump($hex_values);

// Float array
$float_values = array( 0 => 10.2, 1 => 2.4, 2 => -3.4,
                       3 => 0,    4 => 0.5, 5 => 7.3e3, 6 => -9.34E-2);

echo "\n-- Sorting Float array --\n";
var_dump( usort($float_values, 'cmp_function') );
var_dump($float_values);

// empty array
$empty_array = array();

echo "\n-- Sorting empty array --\n";
var_dump( usort($empty_array, 'cmp_function') );
var_dump($empty_array);
?>
===DONE===
--EXPECTF--
*** Testing usort() : usage variation ***

-- Sorting Integer array --
bool(true)
array(9) {
  [0]=>
  int(-1200)
  [1]=>
  int(-3)
  [2]=>
  int(0)
  [3]=>
  int(2)
  [4]=>
  int(3)
  [5]=>
  int(25)
  [6]=>
  int(100)
  [7]=>
  int(150)
  [8]=>
  int(350)
}

-- Sorting Octal array --
bool(true)
array(7) {
  [0]=>
  int(-37)
  [1]=>
  int(-7)
  [2]=>
  int(0)
  [3]=>
  int(1)
  [4]=>
  int(13)
  [5]=>
  int(19)
  [6]=>
  int(46)
}

-- Sorting Hex array --
bool(true)
array(6) {
  [0]=>
  int(-242)
  [1]=>
  int(-207)
  [2]=>
  int(16)
  [3]=>
  int(18)
  [4]=>
  int(43)
  [5]=>
  int(174)
}

-- Sorting Float array --
bool(true)
array(7) {
  [0]=>
  float(-3.4)
  [1]=>
  float(-0.0934)
  [2]=>
  int(0)
  [3]=>
  float(0.5)
  [4]=>
  float(2.4)
  [5]=>
  float(10.2)
  [6]=>
  float(7300)
}

-- Sorting empty array --
bool(true)
array(0) {
}
===DONE===
