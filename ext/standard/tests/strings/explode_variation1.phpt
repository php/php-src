--TEST--
Test explode() function : usage variations - test values for $delimiter argument
--FILE--
<?php

/* Prototype  : array explode  ( string $delimiter  , string $string  [, int $limit  ] )
 * Description: Split a string by string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing explode() function: with unexpected inputs for 'delimiter' argument ***\n";

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

// array with different values for $delimiter
$delimiters =  array (

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

// loop through with each element of the $delimiters array to test explode() function
$count = 1;
$string = "piece1 piece2 piece3 piece4 piece5 piece6";
$limit = 5;
foreach($delimiters as $delimiter) {
  echo "-- Iteration $count --\n";
  var_dump( explode($delimiter, $string, $limit) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing explode() function: with unexpected inputs for 'delimiter' argument ***
-- Iteration 1 --
array(1) {
  [0]=>
  string(41) "piece1 piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 2 --
array(2) {
  [0]=>
  string(5) "piece"
  [1]=>
  string(35) " piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 3 --
array(1) {
  [0]=>
  string(41) "piece1 piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 4 --
array(1) {
  [0]=>
  string(41) "piece1 piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 5 --
array(1) {
  [0]=>
  string(41) "piece1 piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 6 --
array(1) {
  [0]=>
  string(41) "piece1 piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 7 --
array(1) {
  [0]=>
  string(41) "piece1 piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 8 --
array(1) {
  [0]=>
  string(41) "piece1 piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 9 --
array(1) {
  [0]=>
  string(41) "piece1 piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 10 --

Warning: explode() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: explode() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: explode() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --
array(2) {
  [0]=>
  string(5) "piece"
  [1]=>
  string(35) " piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 14 --

Warning: explode(): Empty delimiter in %s on line %d
bool(false)
-- Iteration 15 --
array(2) {
  [0]=>
  string(5) "piece"
  [1]=>
  string(35) " piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 16 --

Warning: explode(): Empty delimiter in %s on line %d
bool(false)
-- Iteration 17 --

Warning: explode(): Empty delimiter in %s on line %d
bool(false)
-- Iteration 18 --

Warning: explode(): Empty delimiter in %s on line %d
bool(false)
-- Iteration 19 --
array(1) {
  [0]=>
  string(41) "piece1 piece2 piece3 piece4 piece5 piece6"
}
-- Iteration 20 --

Warning: explode() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 21 --

Warning: explode(): Empty delimiter in %s on line %d
bool(false)
-- Iteration 22 --

Warning: explode(): Empty delimiter in %s on line %d
bool(false)
===DONE===
