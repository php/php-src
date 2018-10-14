--TEST--
Test str_pad() function : usage variations - unexpected inputs for '$input' argument
--FILE--
<?php
/* Prototype  : string str_pad  ( string $input  , int $pad_length  [, string $pad_string  [, int $pad_type  ]] )
 * Description: Pad a string to a certain length with another string
 * Source code: ext/standard/string.c
*/

/* Test str_pad() function: with unexpected inputs for '$input'
 *  and expected type for '$pad_length'
*/

echo "*** Testing str_pad() function: with unexpected inputs for 'input' argument ***\n";

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

//defining '$pad_length' argument
$pad_length = "20";

// loop through with each element of the $inputs array to test str_pad() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  var_dump( str_pad($input, $pad_length) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing str_pad() function: with unexpected inputs for 'input' argument ***
-- Iteration 1 --
string(20) "0                   "
-- Iteration 2 --
string(20) "1                   "
-- Iteration 3 --
string(20) "-2                  "
-- Iteration 4 --
string(20) "2147483647          "
-- Iteration 5 --
string(20) "-2147483648         "
-- Iteration 6 --
string(20) "10.5                "
-- Iteration 7 --
string(20) "-20.5               "
-- Iteration 8 --
string(20) "101234567000        "
-- Iteration 9 --

Warning: str_pad() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 10 --

Warning: str_pad() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: str_pad() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --
string(20) "1                   "
-- Iteration 13 --
string(20) "                    "
-- Iteration 14 --
string(20) "1                   "
-- Iteration 15 --
string(20) "                    "
-- Iteration 16 --
string(20) "                    "
-- Iteration 17 --
string(20) "                    "
-- Iteration 18 --
string(20) "sample object       "
-- Iteration 19 --

Warning: str_pad() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 20 --
string(20) "                    "
-- Iteration 21 --
string(20) "                    "
===DONE===
