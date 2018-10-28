--TEST--
Test strlen() function : usage variations - unexpected input for '$string' argument
--FILE--
<?php

/* Prototype  : int strlen  ( string $string  )
 * Description: Get string length
 * Source code: ext/standard/string.c
*/

echo "*** Testing strlen() : with unexpected input for 'string' argument ***\n";

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
$pad_length = "20";

// loop through with each element of the $inputs array to test strlen() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( strlen($input) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing strlen() : with unexpected input for 'string' argument ***
-- Iteration 1 --
int(1)
-- Iteration 2 --
int(1)
-- Iteration 3 --
int(2)
-- Iteration 4 --
int(10)
-- Iteration 5 --
int(11)
-- Iteration 6 --
int(4)
-- Iteration 7 --
int(5)
-- Iteration 8 --
int(12)
-- Iteration 9 --

Warning: strlen() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 10 --

Warning: strlen() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: strlen() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --
int(1)
-- Iteration 13 --
int(0)
-- Iteration 14 --
int(1)
-- Iteration 15 --
int(0)
-- Iteration 16 --
int(0)
-- Iteration 17 --
int(0)
-- Iteration 18 --
int(13)
-- Iteration 19 --

Warning: strlen() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 20 --
int(0)
-- Iteration 21 --
int(0)
===DONE===
