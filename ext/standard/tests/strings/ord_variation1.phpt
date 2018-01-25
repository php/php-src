--TEST--
Test ord() function : usage variations - test values for $string argument
--FILE--
<?php

/* Prototype  : int ord  ( string $string  )
 * Description: Return ASCII value of character
 * Source code: ext/standard/string.c
*/

echo "*** Testing ord() function: with unexpected inputs for 'string' argument ***\n";

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

// loop through with each element of the $string array to test ord() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( ord($input) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing ord() function: with unexpected inputs for 'string' argument ***
-- Iteration 1 --
int(48)
-- Iteration 2 --
int(49)
-- Iteration 3 --
int(50)
-- Iteration 4 --
int(50)
-- Iteration 5 --
int(50)
-- Iteration 6 --
int(45)
-- Iteration 7 --
int(49)
-- Iteration 8 --
int(45)
-- Iteration 9 --
int(49)
-- Iteration 10 --

Warning: ord() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: ord() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: ord() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --
int(49)
-- Iteration 14 --
int(0)
-- Iteration 15 --
int(49)
-- Iteration 16 --
int(0)
-- Iteration 17 --
int(0)
-- Iteration 18 --
int(0)
-- Iteration 19 --
int(115)
-- Iteration 20 --

Warning: ord() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 21 --
int(0)
-- Iteration 22 --
int(0)
===DONE===
