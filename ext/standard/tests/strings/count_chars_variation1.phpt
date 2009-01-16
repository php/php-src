--TEST--
Test count_chars() function : usage variations - test values for $string argument
--FILE--
<?php

/* Prototype  : mixed count_chars  ( string $string  [, int $mode  ] )
 * Description: Return information about characters used in a string
 * Source code: ext/standard/string.c
*/

echo "*** Testing count_chars() function: with unexpected inputs for 'string' argument ***\n";

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
/* 1 */		  0,
			  1,
			  255,
			  256,
	    	  2147483647,
		 	 -2147483648,
			
			  // float values
/* 7 */		  10.5,
			  -20.5,
			  10.1234567e10,
			
			  // array values
/* 10 */	  array(),
			  array(0),
			  array(1, 2),
			
			  // boolean values
/* 13 */	  true,
			  false,
			  TRUE,
			  FALSE,
			
			  // null values
/* 17 */	  NULL,
			  null,
			
			  // objects
/* 19 */	  new sample(),
			
			  // resource
/* 20 */	  $file_handle,
			
			  // undefined variable
/* 21 */	  @$undefined_var,
			
			  // unset variable
/* 22 */	  @$unset_var
);

// loop through with each element of the $inputs array to test count_chars() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  // only list characters with a frequency > 0
  var_dump(count_chars($input, 1));
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing count_chars() function: with unexpected inputs for 'string' argument ***
-- Iteration 1 --
array(1) {
  [0]=>
  int(1)
}
-- Iteration 2 --
array(1) {
  [1]=>
  int(1)
}
-- Iteration 3 --
array(2) {
  [2]=>
  int(1)
  [5]=>
  int(2)
}
-- Iteration 4 --
array(3) {
  [2]=>
  int(1)
  [5]=>
  int(1)
  [6]=>
  int(1)
}
-- Iteration 5 --
array(7) {
  [2]=>
  int(1)
  [1]=>
  int(1)
  [4]=>
  int(3)
  [7]=>
  int(2)
  [8]=>
  int(1)
  [3]=>
  int(1)
  [6]=>
  int(1)
}
-- Iteration 6 --
array(8) {
  [u"-"]=>
  int(1)
  [2]=>
  int(1)
  [1]=>
  int(1)
  [4]=>
  int(3)
  [7]=>
  int(1)
  [8]=>
  int(2)
  [3]=>
  int(1)
  [6]=>
  int(1)
}
-- Iteration 7 --
array(4) {
  [1]=>
  int(1)
  [0]=>
  int(1)
  [u"."]=>
  int(1)
  [5]=>
  int(1)
}
-- Iteration 8 --
array(5) {
  [u"-"]=>
  int(1)
  [2]=>
  int(1)
  [0]=>
  int(1)
  [u"."]=>
  int(1)
  [5]=>
  int(1)
}
-- Iteration 9 --
array(8) {
  [1]=>
  int(2)
  [0]=>
  int(4)
  [2]=>
  int(1)
  [3]=>
  int(1)
  [4]=>
  int(1)
  [5]=>
  int(1)
  [6]=>
  int(1)
  [7]=>
  int(1)
}
-- Iteration 10 --

Warning: count_chars() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: count_chars() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --

Warning: count_chars() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 13 --
array(1) {
  [1]=>
  int(1)
}
-- Iteration 14 --
array(0) {
}
-- Iteration 15 --
array(1) {
  [1]=>
  int(1)
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
array(12) {
  [u"s"]=>
  int(1)
  [u"a"]=>
  int(1)
  [u"m"]=>
  int(1)
  [u"p"]=>
  int(1)
  [u"l"]=>
  int(1)
  [u"e"]=>
  int(2)
  [u" "]=>
  int(1)
  [u"o"]=>
  int(1)
  [u"b"]=>
  int(1)
  [u"j"]=>
  int(1)
  [u"c"]=>
  int(1)
  [u"t"]=>
  int(1)
}
-- Iteration 20 --

Warning: count_chars() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL
-- Iteration 21 --
array(0) {
}
-- Iteration 22 --
array(0) {
}
===DONE===
