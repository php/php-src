--TEST--
Test sscanf() function : usage variations - unexpected inputs for '$format' argument
--FILE--
<?php
/* Prototype  : mixed sscanf  ( string $str  , string $format  [, mixed &$...  ] )
 * Description: Parses input from a string according to a format
 * Source code: ext/standard/string.c
*/

echo "*** Testing sscanf() function: with unexpected inputs for 'format' argument ***\n";

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
		  -2,
		   2147483647,
		  -2147483648,

		  // float values
/*6*/	  10.5,
		  -20.5,
		  10.1234567e10,

		  // array values
/*9*/	  array(),
		  array(0),
		  array(1, 2),

		  // boolean values
/*12*/	  true,
		  false,
		  TRUE,
		  FALSE,

		  // null values
/*16*/	  NULL,
		  null,

		  // objects
/*18*/	  new sample(),

		  // resource
/*19*/	  $file_handle,

		  // undefined variable
/*20*/	  @$undefined_var,

		  // unset variable
/*21*/	  @$unset_var
);

//defining '$pad_length' argument
$str = "Hello World";

// loop through with each element of the $inputs array to test sscanf() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( sscanf($str, $input) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing sscanf() function: with unexpected inputs for 'format' argument ***
-- Iteration 1 --
array(0) {
}
-- Iteration 2 --
array(0) {
}
-- Iteration 3 --
array(0) {
}
-- Iteration 4 --
array(0) {
}
-- Iteration 5 --
array(0) {
}
-- Iteration 6 --
array(0) {
}
-- Iteration 7 --
array(0) {
}
-- Iteration 8 --
array(0) {
}
-- Iteration 9 --

Warning: sscanf() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 10 --

Warning: sscanf() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: sscanf() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 12 --
array(0) {
}
-- Iteration 13 --
array(0) {
}
-- Iteration 14 --
array(0) {
}
-- Iteration 15 --
array(0) {
}
-- Iteration 16 --
array(0) {
}
-- Iteration 17 --
array(0) {
}
-- Iteration 18 --
array(0) {
}
-- Iteration 19 --

Warning: sscanf() expects parameter 2 to be string, resource given in %s on line %d
NULL
-- Iteration 20 --
array(0) {
}
-- Iteration 21 --
array(0) {
}
===DONE===
