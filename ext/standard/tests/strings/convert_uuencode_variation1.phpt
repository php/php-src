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

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 2 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 3 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 4 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 5 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 6 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 7 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 8 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 9 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 10 --

Warning: convert_uuencode() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
unicode(0) ""
-- Iteration 11 --

Warning: convert_uuencode() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
unicode(0) ""
-- Iteration 12 --

Warning: convert_uuencode() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
unicode(0) ""
-- Iteration 13 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 14 --
unicode(0) ""
-- Iteration 15 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 16 --
unicode(0) ""
-- Iteration 17 --
unicode(0) ""
-- Iteration 18 --
unicode(0) ""
-- Iteration 19 --

Warning: bin2hex() expects parameter 1 to be strictly a binary string, Unicode string given in %s on line %d
NULL
-- Iteration 20 --

Warning: convert_uuencode() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
unicode(0) ""
-- Iteration 21 --
unicode(0) ""
-- Iteration 22 --
unicode(0) ""
===DONE===
