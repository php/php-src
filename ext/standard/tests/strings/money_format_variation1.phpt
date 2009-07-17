--TEST--
Test money_format() function :  usage variations - test values for $format argument
--SKIPIF--
<?php
	if (!function_exists('money_format')) {
		die("SKIP money_format - not supported\n");
	}
?>
--FILE--
<?php
/* Prototype  : string money_format  ( string $format  , float $number  )
 * Description: Formats a number as a currency string
 * Source code: ext/standard/string.c
*/

// ===========================================================================================
// = We do not test for exact return-values, as those might be different between OS-versions =
// ===========================================================================================

echo "*** Testing money_format() function: with unexpected inputs for 'format' argument ***\n";

//get an unset variable
$unset_var = '  string_val  ';
unset($unset_var);

//defining a couple of sample classes
class class_no_tostring  {
}
 
class class_with_tostring  {
  public function __toString() {
    return "  sample object  ";
  } 
}


//getting the resource
$file_handle = fopen(__FILE__, "r");

// array with different values for $input
$formats =  array (

		  // integer values
/*1*/	  0,
		  1,
		  255,
		  256,
	      2147483647,
		  -2147483648,
		
		  // float values
/*7*/	  0.5,
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
		  
		  // string values
/*19*/	  "abcd",
		  'abcd',
		  "0x12f",
		  "%=*!14#8.2nabcd",
		
		  // objects
/*23*/	  new class_no_tostring(),
		  new class_with_tostring(),
		
		  // resource
/*25*/	  $file_handle,
		
		  // undefined variable
/*26*/	  @$undefined_var,
		
		  // unset variable
/*27*/	  @$unset_var
);

// loop through with each element of the $formats array to test money_format() function
$count = 1;
$value = 1234.56;

foreach($formats as $format) {
  echo "-- Iteration $count --\n";
  echo gettype( money_format($format, $value) )."\n";
  $count ++;
}

// close the file handle
fclose($file_handle);  
?>
===DONE===
--EXPECTF--
*** Testing money_format() function: with unexpected inputs for 'format' argument ***
-- Iteration 1 --
unicode
-- Iteration 2 --
unicode
-- Iteration 3 --
unicode
-- Iteration 4 --
unicode
-- Iteration 5 --
unicode
-- Iteration 6 --
unicode
-- Iteration 7 --
unicode
-- Iteration 8 --
unicode
-- Iteration 9 --
unicode
-- Iteration 10 --

Warning: money_format() expects parameter 1 to be binary string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: money_format() expects parameter 1 to be binary string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: money_format() expects parameter 1 to be binary string, array given in %s on line %d
NULL
-- Iteration 13 --
unicode
-- Iteration 14 --
unicode
-- Iteration 15 --
unicode
-- Iteration 16 --
unicode
-- Iteration 17 --
unicode
-- Iteration 18 --
unicode
-- Iteration 19 --
unicode
-- Iteration 20 --
unicode
-- Iteration 21 --
unicode
-- Iteration 22 --
unicode
-- Iteration 23 --

Warning: money_format() expects parameter 1 to be binary string, object given in %s on line %d
NULL
-- Iteration 24 --
unicode
-- Iteration 25 --

Warning: money_format() expects parameter 1 to be binary string, resource given in %s on line %d
NULL
-- Iteration 26 --
unicode
-- Iteration 27 --
unicode
===DONE===