--TEST--
Test rtrim() function : usage variations - test values for $str argument
--FILE--
<?php

/* Prototype  : string rtrim  ( string $str  [, string $charlist  ] )
 * Description: Strip whitespace (or other characters) from the end of a string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing rtrim() function: with unexpected inputs for 'str' argument ***\n";

//get an unset variable
$unset_var = '  !--string_val--!  ';
unset($unset_var);

//defining a class
class sample  {
  public function __toString() {
    return "  !---sample object---!  ";
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

// loop through with each element of the $inputs array to test rtrim() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  // strip white space and any "minus" signs
  var_dump( rtrim($input, " !-") );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing rtrim() function: with unexpected inputs for 'str' argument ***
-- Iteration 1 --
unicode(1) "0"
-- Iteration 2 --
unicode(1) "1"
-- Iteration 3 --
unicode(3) "255"
-- Iteration 4 --
unicode(3) "256"
-- Iteration 5 --
unicode(10) "2147483647"
-- Iteration 6 --
unicode(11) "-2147483648"
-- Iteration 7 --
unicode(4) "10.5"
-- Iteration 8 --
unicode(5) "-20.5"
-- Iteration 9 --
unicode(12) "101234567000"
-- Iteration 10 --

Warning: rtrim() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: rtrim() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --

Warning: rtrim() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 13 --
unicode(1) "1"
-- Iteration 14 --
unicode(0) ""
-- Iteration 15 --
unicode(1) "1"
-- Iteration 16 --
unicode(0) ""
-- Iteration 17 --
unicode(0) ""
-- Iteration 18 --
unicode(0) ""
-- Iteration 19 --
unicode(19) "  !---sample object"
-- Iteration 20 --

Warning: rtrim() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL
-- Iteration 21 --
unicode(0) ""
-- Iteration 22 --
unicode(0) ""
===DONE===