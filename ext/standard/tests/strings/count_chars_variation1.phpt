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
  [48]=>
  int(1)
}
-- Iteration 2 --
array(1) {
  [49]=>
  int(1)
}
-- Iteration 3 --
array(2) {
  [50]=>
  int(1)
  [53]=>
  int(2)
}
-- Iteration 4 --
array(3) {
  [50]=>
  int(1)
  [53]=>
  int(1)
  [54]=>
  int(1)
}
-- Iteration 5 --
array(7) {
  [49]=>
  int(1)
  [50]=>
  int(1)
  [51]=>
  int(1)
  [52]=>
  int(3)
  [54]=>
  int(1)
  [55]=>
  int(2)
  [56]=>
  int(1)
}
-- Iteration 6 --
array(8) {
  [45]=>
  int(1)
  [49]=>
  int(1)
  [50]=>
  int(1)
  [51]=>
  int(1)
  [52]=>
  int(3)
  [54]=>
  int(1)
  [55]=>
  int(1)
  [56]=>
  int(2)
}
-- Iteration 7 --
array(4) {
  [46]=>
  int(1)
  [48]=>
  int(1)
  [49]=>
  int(1)
  [53]=>
  int(1)
}
-- Iteration 8 --
array(5) {
  [45]=>
  int(1)
  [46]=>
  int(1)
  [48]=>
  int(1)
  [50]=>
  int(1)
  [53]=>
  int(1)
}
-- Iteration 9 --
array(8) {
  [48]=>
  int(4)
  [49]=>
  int(2)
  [50]=>
  int(1)
  [51]=>
  int(1)
  [52]=>
  int(1)
  [53]=>
  int(1)
  [54]=>
  int(1)
  [55]=>
  int(1)
}
-- Iteration 10 --

Warning: count_chars() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: count_chars() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: count_chars() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --
array(1) {
  [49]=>
  int(1)
}
-- Iteration 14 --
array(0) {
}
-- Iteration 15 --
array(1) {
  [49]=>
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
  [32]=>
  int(1)
  [97]=>
  int(1)
  [98]=>
  int(1)
  [99]=>
  int(1)
  [101]=>
  int(2)
  [106]=>
  int(1)
  [108]=>
  int(1)
  [109]=>
  int(1)
  [111]=>
  int(1)
  [112]=>
  int(1)
  [115]=>
  int(1)
  [116]=>
  int(1)
}
-- Iteration 20 --

Warning: count_chars() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 21 --
array(0) {
}
-- Iteration 22 --
array(0) {
}
===DONE===
