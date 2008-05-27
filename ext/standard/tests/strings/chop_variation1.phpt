--TEST--
Test chop() function : usage variations  - unexpected values for str argument 
--FILE--
<?php
/* Prototype  : string chop ( string $str [, string $charlist] )
 * Description: Strip whitespace (or other characters) from the end of a string
 * Source code: ext/standard/string.c
*/

/*
 * Testing chop() : with different unexpected values for str argument passed to the function 
*/

echo "*** Testing chop() : with unexpected values for str argument passed to the function ***\n";
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
// when $str arugment is supplied with different values

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
*** Testing chop() : with unexpected values for str argument passed to the function ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing chop() by supplying different values for 'str' argument ---
-- Iteration 1 --
unicode(1) "0"
unicode(0) ""
-- Iteration 2 --
unicode(1) "1"
unicode(0) ""
-- Iteration 3 --
unicode(5) "12345"
unicode(0) ""
-- Iteration 4 --
unicode(5) "-2345"
unicode(1) "-"
-- Iteration 5 --
unicode(4) "10.5"
unicode(3) "10."
-- Iteration 6 --
unicode(5) "-10.5"
unicode(4) "-10."
-- Iteration 7 --
unicode(12) "101234567000"
unicode(0) ""
-- Iteration 8 --
unicode(13) "1.07654321E-9"
unicode(12) "1.07654321E-"
-- Iteration 9 --
unicode(3) "0.5"
unicode(2) "0."
-- Iteration 10 --

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 13 --

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 14 --

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 15 --
unicode(1) "1"
unicode(0) ""
-- Iteration 16 --
unicode(0) ""
unicode(0) ""
-- Iteration 17 --
unicode(1) "1"
unicode(0) ""
-- Iteration 18 --
unicode(0) ""
unicode(0) ""
-- Iteration 19 --
unicode(0) ""
unicode(0) ""
-- Iteration 20 --
unicode(0) ""
unicode(0) ""
-- Iteration 21 --
unicode(0) ""
unicode(0) ""
-- Iteration 22 --
unicode(0) ""
unicode(0) ""
-- Iteration 23 --
unicode(0) ""
unicode(0) ""
-- Iteration 24 --
unicode(0) ""
unicode(0) ""
-- Iteration 25 --
unicode(16) " @#$%Object @#$%"
unicode(11) " @#$%Object"
-- Iteration 26 --

Warning: chop() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL

Warning: chop() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL
Done
