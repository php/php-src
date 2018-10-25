--TEST--
Test bin2hex() function : usage variations - test values for $str argument
--FILE--
<?php

/* Prototype  : string bin2hex  ( string $str  )
 * Description: Convert binary data into hexadecimal representation
 * Source code: ext/standard/string.c
*/

echo "*** Testing bin2hex() function: with unexpected inputs for 'str' argument ***\n";

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
/*1*/ 0,
	  1,
	  123456,

	  // float values
/*4*/ 10.5,
	  -20.5,
	  10.1234567e10,

	  // array values
/*7*/ array(),
	  array(0),
	  array(1, 2),

	  // boolean values
/*10*/true,
	  false,
	  TRUE,
	  FALSE,

	  // null values
/*14*/NULL,
	  null,

	  // objects
/*16*/new sample(),

	  // resource
/*17*/$file_handle,

	  // undefined variable
/*18*/@$undefined_var,

	  // unset variable
/*19*/@$unset_var
);

// loop through with each element of the $inputs array to test bin2hex() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump(bin2hex($input) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing bin2hex() function: with unexpected inputs for 'str' argument ***
-- Iteration 1 --
string(2) "30"
-- Iteration 2 --
string(2) "31"
-- Iteration 3 --
string(12) "313233343536"
-- Iteration 4 --
string(8) "31302e35"
-- Iteration 5 --
string(10) "2d32302e35"
-- Iteration 6 --
string(24) "313031323334353637303030"
-- Iteration 7 --

Warning: bin2hex() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 8 --

Warning: bin2hex() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 9 --

Warning: bin2hex() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 10 --
string(2) "31"
-- Iteration 11 --
string(0) ""
-- Iteration 12 --
string(2) "31"
-- Iteration 13 --
string(0) ""
-- Iteration 14 --
string(0) ""
-- Iteration 15 --
string(0) ""
-- Iteration 16 --
string(26) "73616d706c65206f626a656374"
-- Iteration 17 --

Warning: bin2hex() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 18 --
string(0) ""
-- Iteration 19 --
string(0) ""
===DONE===
