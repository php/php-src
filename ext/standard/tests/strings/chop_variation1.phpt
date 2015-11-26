--TEST--
Test chop() function : usage variations  - unexpected values for str argument 
--FILE--
<?php
/* Prototype  : string chop ( string $str [, string $charlist] )
 * Description: Strip whitespace (or other characters) from the end of a string
 * Source code: ext/standard/string.c
*/

/*
 * Testing chop() : with different unexpected values for $str argument passed to the function
*/

echo "*** Testing chop() : with unexpected values for str argument ***\n";
// initialize all required variables

$charlist = " @#$%1234567890";
// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// declaring class
class sample  {
  public function __toString() {
    return " @#$%Object @#$%";
  }
}
$sample_obj = new sample;

// creating a file resource
$file_handle = fopen(__FILE__, 'r');

// array with different values
$values =  array (

  // integer values
  0,
  1,
  12345,
  -2345,

  // float values
  10.5,
  -10.5,
  10.1234567e10,
  10.7654321E-10,
  .5,

  // array values
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // boolean values
  true,
  false,
  TRUE,
  FALSE,

  // empty string
  "",
  '',

  // null vlaues
  NULL,
  null,

  // undefined variable
  $undefined_var,

  // unset variable
  $unset_var,
 
  // object
  $sample_obj,
  
  // resource
  $file_handle
);


// loop through each element of the array and check the working of chop()
// when $str argument is supplied with different values

echo "\n--- Testing chop() by supplying different values for 'str' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $str = $values [$index];

  var_dump( chop($str) );
  var_dump( chop($str, $charlist) );

  $counter ++;
}

// closing the resource
fclose( $file_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing chop() : with unexpected values for str argument ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing chop() by supplying different values for 'str' argument ---
-- Iteration 1 --
string(1) "0"
string(0) ""
-- Iteration 2 --
string(1) "1"
string(0) ""
-- Iteration 3 --
string(5) "12345"
string(0) ""
-- Iteration 4 --
string(5) "-2345"
string(1) "-"
-- Iteration 5 --
string(4) "10.5"
string(3) "10."
-- Iteration 6 --
string(5) "-10.5"
string(4) "-10."
-- Iteration 7 --
string(12) "101234567000"
string(0) ""
-- Iteration 8 --
string(13) "1.07654321E-9"
string(12) "1.07654321E-"
-- Iteration 9 --
string(3) "0.5"
string(2) "0."
-- Iteration 10 --

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
string(1) "1"
string(0) ""
-- Iteration 16 --
string(0) ""
string(0) ""
-- Iteration 17 --
string(1) "1"
string(0) ""
-- Iteration 18 --
string(0) ""
string(0) ""
-- Iteration 19 --
string(0) ""
string(0) ""
-- Iteration 20 --
string(0) ""
string(0) ""
-- Iteration 21 --
string(0) ""
string(0) ""
-- Iteration 22 --
string(0) ""
string(0) ""
-- Iteration 23 --
string(0) ""
string(0) ""
-- Iteration 24 --
string(0) ""
string(0) ""
-- Iteration 25 --
string(16) " @#$%Object @#$%"
string(11) " @#$%Object"
-- Iteration 26 --

Warning: chop() expects parameter 1 to be string, resource given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string, resource given in %s on line %d
NULL
Done
