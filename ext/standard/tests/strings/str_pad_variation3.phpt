--TEST--
Test str_pad() function : usage variations - unexpected inputs for '$pad_string' argument
--FILE--
<?php
/* Prototype  : string str_pad  ( string $input  , int $pad_length  [, string $pad_string  [, int $pad_type  ]] )
 * Description: Pad a string to a certain length with another string
 * Source code: ext/standard/string.c
*/

/* Test str_pad() function: with unexpected inputs for '$pad_string'
 *  and expected type for '$input' and '$pad_length'
*/

echo "*** Testing str_pad() function: with unexpected inputs for 'pad_string' argument ***\n";

//get an unset variable
$unset_var = 'string_val';
unset($unset_var);

//defining a class
class sample  {
  public function __toString() {
    return "sample object";
  }
}

//getting the resource
$file_handle = fopen(__FILE__, "r");

// array with different values for $input
$pad_strings =  array (

		  // integer values
/*1*/	  0,
		  1,
		  -2,
		  2147483647,
		  -2147483648,

		  // float values
/*6*/	  10.5,
		  -20.5,
		  10.1234567e10,

		  // array values
/*9*/	  array(),
		  array(0),
		  array(1, 2),

		  // boolean values
/*12*/	  true,
		  false,
		  TRUE,
		  FALSE,

		  // null values
/*16*/	  NULL,
		  null,

		  // objects
/*18*/	  new sample(),

		  // resource
/*19*/	  $file_handle,

		  // undefined variable
/*20*/	  @$undefined_var,

		  // unset variable
/*21*/	  @$unset_var
);

//defining '$input' argument
$input = "Test string";
$pad_length = 20;

// loop through with each element of the $pad_strings array to test str_pad() function
$count = 1;
foreach($pad_strings as $pad_string) {
  echo "-- Iteration $count --\n";
  var_dump( str_pad($input, $pad_length, $pad_string) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing str_pad() function: with unexpected inputs for 'pad_string' argument ***
-- Iteration 1 --
string(20) "Test string000000000"
-- Iteration 2 --
string(20) "Test string111111111"
-- Iteration 3 --
string(20) "Test string-2-2-2-2-"
-- Iteration 4 --
string(20) "Test string214748364"
-- Iteration 5 --
string(20) "Test string-21474836"
-- Iteration 6 --
string(20) "Test string10.510.51"
-- Iteration 7 --
string(20) "Test string-20.5-20."
-- Iteration 8 --
string(20) "Test string101234567"
-- Iteration 9 --

Warning: str_pad() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 10 --

Warning: str_pad() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: str_pad() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 12 --
string(20) "Test string111111111"
-- Iteration 13 --

Warning: str_pad(): Padding string cannot be empty in %s on line %d
NULL
-- Iteration 14 --
string(20) "Test string111111111"
-- Iteration 15 --

Warning: str_pad(): Padding string cannot be empty in %s on line %d
NULL
-- Iteration 16 --

Warning: str_pad(): Padding string cannot be empty in %s on line %d
NULL
-- Iteration 17 --

Warning: str_pad(): Padding string cannot be empty in %s on line %d
NULL
-- Iteration 18 --
string(20) "Test stringsample ob"
-- Iteration 19 --

Warning: str_pad() expects parameter 3 to be string, resource given in %s on line %d
NULL
-- Iteration 20 --

Warning: str_pad(): Padding string cannot be empty in %s on line %d
NULL
-- Iteration 21 --

Warning: str_pad(): Padding string cannot be empty in %s on line %d
NULL
===DONE===
