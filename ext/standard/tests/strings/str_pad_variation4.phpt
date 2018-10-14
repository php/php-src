--TEST--
Test str_pad() function : usage variations - unexpected inputs for '$pad_type' argument
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
--FILE--
<?php
/* Prototype  : string str_pad  ( string $input  , int $pad_length  [, string $pad_string  [, int $pad_type  ]] )
 * Description: Pad a string to a certain length with another string
 * Source code: ext/standard/string.c
*/

/* Test str_pad() function: with unexpected inputs for '$pad_type'
 *  and expected type for '$input', '$pad_length' and '$pad_string'
*/

echo "*** Testing str_pad() function: with unexpected inputs for 'pad_type' argument ***\n";

//get an unset variable
$unset_var = 'string_val';
unset($unset_var);

//defining a class
class sample  {
  public function __toString() {
    return "sample object";
  }
}

// array with different values for $input
$pad_types =  array (

		  // integer values
/*1*/	  0, // == STR_PAD_LEFT
		  1, // == STR_PAD_RIGHT
		  2, // == STR_PAD_BOTH
		  -2,
		  2147483647,
		  -2147483648,

		  // float values
/*7*/	  10.5,
		  -20.5,
		  10.1234567e10,

		  // string data
/*10*/	  "abc",
		  "STR_PAD_LEFT",
		  "2",
		  "0x2",
		  "02",

		  // array values
/*15*/	  array(),
		  array(0),
		  array(1, 2),

		  // boolean values
/*18*/	  true,
		  false,
		  TRUE,
		  FALSE,

		  // null vlaues
/*22*/	  NULL,
		  null,

		  // objects
/*24*/	  new sample(),

		  // undefined variable
/*25*/	  @$undefined_var,

		  // unset variable
/*26*/	  @$unset_var
);

//defining '$input' argument
$input = "Test string";
$pad_length = 20;
$pad_string = "*";

// loop through with each element of the $pad_types array to test str_pad() function
$count = 1;
foreach($pad_types as $pad_type) {
  echo "-- Iteration $count --\n";
  var_dump( str_pad($input, $pad_length, $pad_string, $pad_type) );
  $count ++;
}

?>
===DONE===
--EXPECTF--
*** Testing str_pad() function: with unexpected inputs for 'pad_type' argument ***
-- Iteration 1 --
string(20) "*********Test string"
-- Iteration 2 --
string(20) "Test string*********"
-- Iteration 3 --
string(20) "****Test string*****"
-- Iteration 4 --

Warning: str_pad(): Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH in %s on line %d
NULL
-- Iteration 5 --

Warning: str_pad(): Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH in %s on line %d
NULL
-- Iteration 6 --

Warning: str_pad(): Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH in %s on line %d
NULL
-- Iteration 7 --

Warning: str_pad(): Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH in %s on line %d
NULL
-- Iteration 8 --

Warning: str_pad(): Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH in %s on line %d
NULL
-- Iteration 9 --

Warning: str_pad(): Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH in %s on line %d
NULL
-- Iteration 10 --

Warning: str_pad() expects parameter 4 to be integer, string given in %s on line %d
NULL
-- Iteration 11 --

Warning: str_pad() expects parameter 4 to be integer, string given in %s on line %d
NULL
-- Iteration 12 --
string(20) "****Test string*****"
-- Iteration 13 --

Notice: A non well formed numeric value encountered in %s on line %d
string(20) "*********Test string"
-- Iteration 14 --
string(20) "****Test string*****"
-- Iteration 15 --

Warning: str_pad() expects parameter 4 to be integer, array given in %s on line %d
NULL
-- Iteration 16 --

Warning: str_pad() expects parameter 4 to be integer, array given in %s on line %d
NULL
-- Iteration 17 --

Warning: str_pad() expects parameter 4 to be integer, array given in %s on line %d
NULL
-- Iteration 18 --
string(20) "Test string*********"
-- Iteration 19 --
string(20) "*********Test string"
-- Iteration 20 --
string(20) "Test string*********"
-- Iteration 21 --
string(20) "*********Test string"
-- Iteration 22 --
string(20) "*********Test string"
-- Iteration 23 --
string(20) "*********Test string"
-- Iteration 24 --

Warning: str_pad() expects parameter 4 to be integer, object given in %s on line %d
NULL
-- Iteration 25 --
string(20) "*********Test string"
-- Iteration 26 --
string(20) "*********Test string"
===DONE===
