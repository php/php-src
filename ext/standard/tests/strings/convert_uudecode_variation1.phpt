--TEST--
Test convert_uudecode() function : usage variations - test values for $data argument
--FILE--
<?php

/* Prototype  : string convert_uudecode  ( string $data  )
 * Description: Decode a uuencoded string
 * Source code: ext/standard/uuencode.c
*/

echo "*** Testing convert_uudecode() function: with unexpected inputs for 'data' argument ***\n";

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

// array with different values for $data
$inputs =  array (

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
		  10.1234567e10,

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

// loop through with each element of the $data array to test convert_uudecode() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( convert_uudecode($input) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing convert_uudecode() function: with unexpected inputs for 'data' argument ***
-- Iteration 1 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 2 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 3 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 4 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 5 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 6 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 7 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 8 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 9 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 10 --

Warning: convert_uudecode() expects parameter 1 to be string, array given in %s on line %d
bool(false)
-- Iteration 11 --

Warning: convert_uudecode() expects parameter 1 to be string, array given in %s on line %d
bool(false)
-- Iteration 12 --

Warning: convert_uudecode() expects parameter 1 to be string, array given in %s on line %d
bool(false)
-- Iteration 13 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 14 --
bool(false)
-- Iteration 15 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 16 --
bool(false)
-- Iteration 17 --
bool(false)
-- Iteration 18 --
bool(false)
-- Iteration 19 --

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
-- Iteration 20 --

Warning: convert_uudecode() expects parameter 1 to be string, resource given in %s on line %d
bool(false)
-- Iteration 21 --
bool(false)
-- Iteration 22 --
bool(false)
===DONE===
