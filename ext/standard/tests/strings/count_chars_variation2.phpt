--TEST--
Test count_chars() function : usage variations - test values for $mode argument
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
--FILE--
<?php

/* Prototype  : mixed count_chars  ( string $string  [, int $mode  ] )
 * Description: Return information about characters used in a string
 * Source code: ext/standard/string.c
*/

echo "*** Testing count_chars() function: with unexpected inputs for 'mode' argument ***\n";

//get an unset variable
$unset_var = 'string_val';
unset($unset_var);

//defining a class
class sample  {
}

// array with different values for $input
$inputs =  array (

			  // integer values
/* 1 */		  0,
			  1,
			  255,
			  2147483647,
		      -2147483648,
			
			  // float values
/* 6 */		  0.0,
			  1.3,
			  10.5,
			  -20.5,
			  10.1234567e10,
			
			  // array values
/* 11 */	  array(),
			  array(1, 2, 3, 4, 5, 6, 7, 8, 9),
			
			  // boolean values
/* 14 */	  true,
			  false,
			  TRUE,
			  FALSE,
			
			  // null values
/* 18 */	  NULL,
			  null,
			  
			  // string values
/* 20 */	  "ABCD",
			  'abcd',
			  "1ABC",
			  "5ABC",
			  
			  // objects
/* 24 */ 	  new sample(),
			
			   // undefined variable
/* 25 */	  @$undefined_var,
			
			  // unset variable
/* 26 */	  @$unset_var
);

// loop through with each element of the $inputs array to test count_chars() function
// with unexepcted values for the 'mode' argument
$count = 1;
$string = "Return information about characters used in a string";
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  // only list characters with a frequency > 0
  var_dump(is_array(count_chars($string, $input)));
  $count ++;
}


?>
===DONE===
--EXPECTF--
*** Testing count_chars() function: with unexpected inputs for 'mode' argument ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --

Warning: count_chars(): Unknown mode in %s on line %d
bool(false)
-- Iteration 4 --

Warning: count_chars(): Unknown mode in %s on line %d
bool(false)
-- Iteration 5 --

Warning: count_chars(): Unknown mode in %s on line %d
bool(false)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --

Warning: count_chars(): Unknown mode in %s on line %d
bool(false)
-- Iteration 9 --

Warning: count_chars(): Unknown mode in %s on line %d
bool(false)
-- Iteration 10 --

Warning: count_chars(): Unknown mode in %s on line %d
bool(false)
-- Iteration 11 --

Warning: count_chars() expects parameter 2 to be integer, array given in %s on line %d
bool(false)
-- Iteration 12 --

Warning: count_chars() expects parameter 2 to be integer, array given in %s on line %d
bool(false)
-- Iteration 13 --
bool(true)
-- Iteration 14 --
bool(true)
-- Iteration 15 --
bool(true)
-- Iteration 16 --
bool(true)
-- Iteration 17 --
bool(true)
-- Iteration 18 --
bool(true)
-- Iteration 19 --

Warning: count_chars() expects parameter 2 to be integer, string given in %s on line %d
bool(false)
-- Iteration 20 --

Warning: count_chars() expects parameter 2 to be integer, string given in %s on line %d
bool(false)
-- Iteration 21 --

Notice: A non well formed numeric value encountered in %s on line %d
bool(true)
-- Iteration 22 --

Notice: A non well formed numeric value encountered in %s on line %d

Warning: count_chars(): Unknown mode in %s on line %d
bool(false)
-- Iteration 23 --

Warning: count_chars() expects parameter 2 to be integer, object given in %s on line %d
bool(false)
-- Iteration 24 --
bool(true)
-- Iteration 25 --
bool(true)
===DONE===
