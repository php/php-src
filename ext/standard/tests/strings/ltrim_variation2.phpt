--TEST--
Test ltrim() function : usage variations - test values for $charlist argument
--FILE--
<?php

/* Prototype  : string ltrim  ( string $str  [, string $charlist  ] )
 * Description: Strip whitespace (or other characters) from the beginning of a string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing ltrim() function: with unexpected inputs for 'charlist' argument ***\n";

//get an unset variable
$unset_var = '  string_val  ';
unset($unset_var);

//defining a class
class sample  {
  public function __toString() {
    return "  sample object  ";
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

// loop through with each element of the $inputs array to test ltrim() function
$count = 1;
foreach($inputs as $charlist) {
  echo "-- Iteration $count --\n";
  // strip white space and any "minus" signs
  var_dump( ltrim("!---Hello World---!", $charlist) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing ltrim() function: with unexpected inputs for 'charlist' argument ***
-- Iteration 1 --
string(19) "!---Hello World---!"
-- Iteration 2 --
string(19) "!---Hello World---!"
-- Iteration 3 --
string(19) "!---Hello World---!"
-- Iteration 4 --
string(19) "!---Hello World---!"
-- Iteration 5 --
string(19) "!---Hello World---!"
-- Iteration 6 --
string(19) "!---Hello World---!"
-- Iteration 7 --
string(19) "!---Hello World---!"
-- Iteration 8 --
string(19) "!---Hello World---!"
-- Iteration 9 --
string(19) "!---Hello World---!"
-- Iteration 10 --

Warning: ltrim() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: ltrim() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: ltrim() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 13 --
string(19) "!---Hello World---!"
-- Iteration 14 --
string(19) "!---Hello World---!"
-- Iteration 15 --
string(19) "!---Hello World---!"
-- Iteration 16 --
string(19) "!---Hello World---!"
-- Iteration 17 --
string(19) "!---Hello World---!"
-- Iteration 18 --
string(19) "!---Hello World---!"
-- Iteration 19 --
string(19) "!---Hello World---!"
-- Iteration 20 --

Warning: ltrim() expects parameter 2 to be string, resource given in %s on line %d
NULL
-- Iteration 21 --
string(19) "!---Hello World---!"
-- Iteration 22 --
string(19) "!---Hello World---!"
===DONE===