--TEST--
Test hebrev() function : usage variations - test values for $hebrew_text argument
--FILE--
<?php

/* Prototype  : string hebrev  ( string $hebrew_text  [, int $max_chars_per_line  ] )
 * Description: Convert logical Hebrew text to visual text
 * Source code: ext/standard/string.c
*/

echo "*** Testing hebrev() function: with unexpected inputs for 'hebrew_text' argument ***\n";

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

// array with different values for $hebrew_text
$texts =  array (

		  // integer values
/*1*/	  0,
		  1,
		  255,
		  256,
	      2147483647,
		  -2147483648,
		
		  // float values
/*7*/	  10.5,
		  -20.5,
		  10.1234567e5,
		
		  // array values
/*10*/	  array(),
		  array(0),
		  array(1, 2),
		
		  // boolean values
/*13*/	  true,
		  false,
		  TRUE,
		  FALSE,
		
		  // null values
/*17*/	  NULL,
		  null,
		
		  // objects
/*19*/	  new sample(),
		
		  // resource
/*20*/	  $file_handle,
		
		  // undefined variable
/*21*/	  @$undefined_var,
		
		  // unset variable
/*22*/	  @$unset_var
);

// loop through with each element of the $texts array to test hebrev() function
$count = 1;

foreach($texts as $hebrew_text) {
  echo "-- Iteration $count --\n";
  var_dump( hebrev($hebrew_text) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing hebrev() function: with unexpected inputs for 'hebrew_text' argument ***
-- Iteration 1 --
string(1) "0"
-- Iteration 2 --
string(1) "1"
-- Iteration 3 --
string(3) "255"
-- Iteration 4 --
string(3) "256"
-- Iteration 5 --
string(10) "2147483647"
-- Iteration 6 --
string(11) "-2147483648"
-- Iteration 7 --
string(4) "10.5"
-- Iteration 8 --
string(5) "-20.5"
-- Iteration 9 --
string(10) "1012345.67"
-- Iteration 10 --

Warning: hebrev() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: hebrev() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: hebrev() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --
string(1) "1"
-- Iteration 14 --
bool(false)
-- Iteration 15 --
string(1) "1"
-- Iteration 16 --
bool(false)
-- Iteration 17 --
bool(false)
-- Iteration 18 --
bool(false)
-- Iteration 19 --
string(13) "sample object"
-- Iteration 20 --

Warning: hebrev() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 21 --
bool(false)
-- Iteration 22 --
bool(false)
===DONE===