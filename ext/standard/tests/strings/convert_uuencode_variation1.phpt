--TEST--
Test convert_uuencode() function : usage variations - test values for $data argument
--FILE--
<?php

/* Prototype  : string convert_uuencode  ( string $data  )
 * Description: Uuencode a string
 * Source code: ext/standard/uuencode.c
*/

echo "*** Testing convert_uuencode() function: with unexpected inputs for 'data' argument ***\n";

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

// loop through with each element of the $data array to test convert_uuencode() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( bin2hex(convert_uuencode($input)) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing convert_uuencode() function: with unexpected inputs for 'data' argument ***
-- Iteration 1 --
string(16) "212c6060600a600a"
-- Iteration 2 --
string(16) "212c3060600a600a"
-- Iteration 3 --
string(16) "232c4334550a600a"
-- Iteration 4 --
string(16) "232c4334560a600a"
-- Iteration 5 --
string(40) "2a2c4324542d5330582c5338542d5060600a600a"
-- Iteration 6 --
string(40) "2b2b3328512d233c542e232c562d2340600a600a"
-- Iteration 7 --
string(24) "242c33604e2d3060600a600a"
-- Iteration 8 --
string(24) "252b3328502b4334600a600a"
-- Iteration 9 --
string(40) "2c2c3360512c432c542d3338572c2360500a600a"
-- Iteration 10 --

Warning: convert_uuencode() expects parameter 1 to be string, array given in %s on line %d
string(0) ""
-- Iteration 11 --

Warning: convert_uuencode() expects parameter 1 to be string, array given in %s on line %d
string(0) ""
-- Iteration 12 --

Warning: convert_uuencode() expects parameter 1 to be string, array given in %s on line %d
string(0) ""
-- Iteration 13 --
string(16) "212c3060600a600a"
-- Iteration 14 --
string(0) ""
-- Iteration 15 --
string(16) "212c3060600a600a"
-- Iteration 16 --
string(0) ""
-- Iteration 17 --
string(0) ""
-- Iteration 18 --
string(0) ""
-- Iteration 19 --
string(48) "2d3c56254d3c26514528265d423a4635433d6060600a600a"
-- Iteration 20 --

Warning: convert_uuencode() expects parameter 1 to be string, resource given in %s on line %d
string(0) ""
-- Iteration 21 --
string(0) ""
-- Iteration 22 --
string(0) ""
===DONE===
