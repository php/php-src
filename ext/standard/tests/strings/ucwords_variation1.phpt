--TEST--
Test ucwords() function : usage variations - unexpected input values
--FILE--
<?php
/* Prototype  : string ucwords ( string $str )
 * Description: Uppercase the first character of each word in a string
 * Source code: ext/standard/string.c
*/

/*
 * Test ucwords() by passing different values including scalar and non scalar values 
*/

echo "*** Testing ucwords() : usage variations ***\n";
// initialize all required variables

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

$fp = fopen(__FILE__, "r");

class my
{
  function __toString() {
    return "myString";
  }
}

// array with different values
$values =  array (

  // integer values
  0,
  1,
  12345,
  -2345,

  // hex values 
  0x10,
  0X20,
  0xAA,
  -0XF5,

  // octal values
  0123,
  -0342,

  // float values
  10.5,
  -10.5,
  10.1234567e10,
  10.7654321E-10,
  .5,

  // array values
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // boolean values
  true,
  false,
  TRUE,
  FALSE,

  // objects
  new my(),

  // empty string
  "",
  '',

  //NULL
  NULL,
  null,

  // hex in string 
  "0x123",
  '0x123',
  "0xFF12",
  "-0xFF12",
  
  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var,

  // resource variable
  $fp
);

// loop through each element of the array and check the working of ucwords()
// when $str arugment is supplied with different values
echo "\n--- Testing ucwords() by supplying different values for 'str' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $str = $values [$index];

  var_dump( ucwords($str) );

  $counter ++;
}

// close the file handle
fclose($fp);
echo "Done\n";
?>
--EXPECTF--
*** Testing ucwords() : usage variations ***

--- Testing ucwords() by supplying different values for 'str' argument ---
-- Iteration 1 --
string(1) "0"
-- Iteration 2 --
string(1) "1"
-- Iteration 3 --
string(5) "12345"
-- Iteration 4 --
string(5) "-2345"
-- Iteration 5 --
string(2) "16"
-- Iteration 6 --
string(2) "32"
-- Iteration 7 --
string(3) "170"
-- Iteration 8 --
string(4) "-245"
-- Iteration 9 --
string(2) "83"
-- Iteration 10 --
string(4) "-226"
-- Iteration 11 --
string(4) "10.5"
-- Iteration 12 --
string(5) "-10.5"
-- Iteration 13 --
string(12) "101234567000"
-- Iteration 14 --
string(13) "1.07654321E-9"
-- Iteration 15 --
string(3) "0.5"
-- Iteration 16 --

Warning: ucwords() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 17 --

Warning: ucwords() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 18 --

Warning: ucwords() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 19 --

Warning: ucwords() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 20 --

Warning: ucwords() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 21 --
string(1) "1"
-- Iteration 22 --
string(0) ""
-- Iteration 23 --
string(1) "1"
-- Iteration 24 --
string(0) ""
-- Iteration 25 --
string(8) "MyString"
-- Iteration 26 --
string(0) ""
-- Iteration 27 --
string(0) ""
-- Iteration 28 --
string(0) ""
-- Iteration 29 --
string(0) ""
-- Iteration 30 --
string(5) "0x123"
-- Iteration 31 --
string(5) "0x123"
-- Iteration 32 --
string(6) "0xFF12"
-- Iteration 33 --
string(7) "-0xFF12"
-- Iteration 34 --
string(0) ""
-- Iteration 35 --
string(0) ""
-- Iteration 36 --

Warning: ucwords() expects parameter 1 to be string, resource given in %s on line %d
NULL
Done
