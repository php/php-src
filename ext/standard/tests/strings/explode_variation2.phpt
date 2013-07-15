--TEST--
Test explode() function : usage variations - test values for $string argument
--FILE--
<?php

/* Prototype  : array explode  ( string $delimiter  , string $string  [, int $limit  ] )
 * Description: Split a string by string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing explode() function: with unexpected inputs for 'string' argument ***\n";

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

// array with different values for $string
$strings =  array (

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

// loop through with each element of the $strings array to test explode() function
$count = 1;
$delimiter = " ";
$limit = 5;
foreach($strings as $string) {
  echo "-- Iteration $count --\n";
  var_dump( explode($delimiter, $string, $limit) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing explode() function: with unexpected inputs for 'string' argument ***
-- Iteration 1 --
array(1) {
  [0]=>
  string(1) "0"
}
-- Iteration 2 --
array(1) {
  [0]=>
  string(1) "1"
}
-- Iteration 3 --
array(1) {
  [0]=>
  string(3) "255"
}
-- Iteration 4 --
array(1) {
  [0]=>
  string(3) "256"
}
-- Iteration 5 --
array(1) {
  [0]=>
  string(10) "2147483647"
}
-- Iteration 6 --
array(1) {
  [0]=>
  string(11) "-2147483648"
}
-- Iteration 7 --
array(1) {
  [0]=>
  string(4) "10.5"
}
-- Iteration 8 --
array(1) {
  [0]=>
  string(5) "-20.5"
}
-- Iteration 9 --
array(1) {
  [0]=>
  string(12) "101234567000"
}
-- Iteration 10 --

Warning: explode() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: explode() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: explode() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 13 --
array(1) {
  [0]=>
  string(1) "1"
}
-- Iteration 14 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 15 --
array(1) {
  [0]=>
  string(1) "1"
}
-- Iteration 16 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 17 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 18 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 19 --
array(2) {
  [0]=>
  string(6) "sample"
  [1]=>
  string(6) "object"
}
-- Iteration 20 --

Warning: explode() expects parameter 2 to be string, resource given in %s on line %d
NULL
-- Iteration 21 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 22 --
array(1) {
  [0]=>
  string(0) ""
}
===DONE===
