--TEST--
Test chr() function : usage variations - test values for $ascii argument
--FILE--
<?php

/* Prototype  : string chr  ( int $ascii  )
 * Description: Return a specific character
 * Source code: ext/standard/string.c
*/

echo "*** Testing chr() function: with unexpected inputs for 'ascii' argument ***\n";

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
$inputs =  array (

		  // integer values
/*1*/	  0,
		  1,
		  255,
		  256,
		
		  // float values
/*5*/	  10.5,
		  -20.5,
		  1.1234e6,
		
		  // array values
/*8*/	  array(),
		  array(0),
		  array(1, 2),
		
		  // boolean values
/*11*/	  true,
		  false,
		  TRUE,
		  FALSE,
		
		  // null values
/*15*/	  NULL,
		  null,
		
		  // objects
/*17*/	  new sample(),
		
		  // resource
/*18*/	  $file_handle,
		
		  // undefined variable
/*19*/	  @$undefined_var,
		
		  // unset variable
/*20*/	  @$unset_var
);

// loop through with each element of the $inputs array to test chr() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( bin2hex((binary)chr($input)) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing chr() function: with unexpected inputs for 'ascii' argument ***
-- Iteration 1 --
unicode(2) "00"
-- Iteration 2 --
unicode(2) "01"
-- Iteration 3 --
unicode(2) "ff"
-- Iteration 4 --
unicode(2) "3f"
-- Iteration 5 --
unicode(2) "0a"
-- Iteration 6 --
unicode(4) "3f3f"
-- Iteration 7 --

Warning: Codepoint value cannot be greater than 10FFFF in %s on line %d
unicode(0) ""
-- Iteration 8 --

Warning: chr() expects parameter 1 to be long, array given in %s on line %d
unicode(0) ""
-- Iteration 9 --

Warning: chr() expects parameter 1 to be long, array given in %s on line %d
unicode(0) ""
-- Iteration 10 --

Warning: chr() expects parameter 1 to be long, array given in %s on line %d
unicode(0) ""
-- Iteration 11 --
unicode(2) "01"
-- Iteration 12 --
unicode(2) "00"
-- Iteration 13 --
unicode(2) "01"
-- Iteration 14 --
unicode(2) "00"
-- Iteration 15 --
unicode(2) "00"
-- Iteration 16 --
unicode(2) "00"
-- Iteration 17 --

Warning: chr() expects parameter 1 to be long, object given in %s on line %d
unicode(0) ""
-- Iteration 18 --

Warning: chr() expects parameter 1 to be long, resource given in %s on line %d
unicode(0) ""
-- Iteration 19 --
unicode(2) "00"
-- Iteration 20 --
unicode(2) "00"
===DONE===
