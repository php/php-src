--TEST--
Test str_shuffle() function : usage variations - test values for $haystack argument
--FILE--
<?php

/* Prototype  : string str_shuffle  ( string $str  )
 * Description: Randomly shuffles a string
 * Source code: ext/standard/string.c
*/

echo "*** Testing str_shuffle() function: with unexpected inputs for 'string' argument ***\n";

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
		
		  // null vlaues
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


// loop through with each element of the $inputs array to test str_shuffle() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( str_shuffle($input) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing str_shuffle() function: with unexpected inputs for 'string' argument ***
-- Iteration 1 --
unicode(1) "0"
-- Iteration 2 --
unicode(1) "1"
-- Iteration 3 --
unicode(2) "%s"
-- Iteration 4 --
unicode(10) "%s"
-- Iteration 5 --
unicode(11) "%s"
-- Iteration 6 --
unicode(4) "%s"
-- Iteration 7 --
unicode(5) "%s"
-- Iteration 8 --
unicode(12) "%s"
-- Iteration 9 --

Warning: str_shuffle() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 10 --

Warning: str_shuffle() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: str_shuffle() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --
unicode(1) "1"
-- Iteration 13 --
unicode(0) ""
-- Iteration 14 --
unicode(1) "1"
-- Iteration 15 --
unicode(0) ""
-- Iteration 16 --
unicode(0) ""
-- Iteration 17 --
unicode(0) ""
-- Iteration 18 --
unicode(13) "%s"
-- Iteration 19 --

Warning: str_shuffle() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL
-- Iteration 20 --
unicode(0) ""
-- Iteration 21 --
unicode(0) ""
===DONE===