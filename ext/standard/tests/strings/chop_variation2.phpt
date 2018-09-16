--TEST--
Test chop() function : usage variations  - unexpected values for charlist argument
--FILE--
<?php
/* Prototype  : string chop ( string $str [, string $charlist] )
 * Description: Strip whitespace (or other characters) from the end of a string
 * Source code: ext/standard/string.c
*/

/*
 * Testing chop() : with different unexpected values for charlist argument passes to the function
*/

echo "*** Testing chop() : with different unexpected values for charlist argument ***\n";
// initialize all required variables
$str = 'hello world12345 ';

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// declaring class
class sample  {
  public function __toString()  {
  return "@# $%12345";
  }
}

// defining a resource
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

  // objects
  new sample(),

  // empty string
  "",
  '',

  // null values
  NULL,
  null,

  // resource
  $file_handle,

  // undefined variable
  $undefined_var,

  // unset variable
  $unset_var

);


// loop through each element of the array and check the working of chop()
// when $charlist argument is supplied with different values

echo "\n--- Testing chop() by supplying different values for 'charlist' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $charlist = $values [$index];

  var_dump( chop($str, $charlist) );

  $counter ++;
}

// closing the resource
fclose($file_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing chop() : with different unexpected values for charlist argument ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing chop() by supplying different values for 'charlist' argument ---
-- Iteration 1 --
string(17) "hello world12345 "
-- Iteration 2 --
string(17) "hello world12345 "
-- Iteration 3 --
string(17) "hello world12345 "
-- Iteration 4 --
string(17) "hello world12345 "
-- Iteration 5 --
string(17) "hello world12345 "
-- Iteration 6 --
string(17) "hello world12345 "
-- Iteration 7 --
string(17) "hello world12345 "
-- Iteration 8 --
string(17) "hello world12345 "
-- Iteration 9 --
string(17) "hello world12345 "
-- Iteration 10 --

Warning: chop() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: chop() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: chop() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: chop() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: chop() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
string(17) "hello world12345 "
-- Iteration 16 --
string(17) "hello world12345 "
-- Iteration 17 --
string(17) "hello world12345 "
-- Iteration 18 --
string(17) "hello world12345 "
-- Iteration 19 --
string(11) "hello world"
-- Iteration 20 --
string(17) "hello world12345 "
-- Iteration 21 --
string(17) "hello world12345 "
-- Iteration 22 --
string(17) "hello world12345 "
-- Iteration 23 --
string(17) "hello world12345 "
-- Iteration 24 --

Warning: chop() expects parameter 2 to be string, resource given in %s on line %d
NULL
-- Iteration 25 --
string(17) "hello world12345 "
-- Iteration 26 --
string(17) "hello world12345 "
Done
