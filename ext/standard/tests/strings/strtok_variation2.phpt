--TEST--
Test strtok() function : usage variations - with different token strings
--FILE--
<?php
/* Prototype  : string strtok ( str $str, str $token )
 * Description: splits a string (str) into smaller strings (tokens), with each token being delimited by any character from token
 * Source code: ext/standard/string.c
*/

/*
 * Testing strtok() : with different token strings
*/

echo "*** Testing strtok() : with different token strings ***\n";
// initialize all required variables
$str = 'this testcase test strtok() function ';

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// declaring a class
class sample  {
  public function __toString() {
  return "obj-ect";
  }
} 

// Defining resource
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
  10.5e10,
  10.6E-10,
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

  // null vlaues
  NULL,
  null,

  // undefined variable
  $undefined_var,

  // unset variable
  $unset_var,
 
  // resource
  $file_handle
);


// loop through each element of the array and check the working of strtok()
// when $token argument is supplied with different values

echo "\n--- Testing strtok() by supplying different values for 'token' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $token = $values [$index];

  var_dump( strtok($str, $token) );

  $counter ++;
}

// closing the resource
fclose($file_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing strtok() : with different token strings ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing strtok() by supplying different values for 'token' argument ---
-- Iteration 1 --
string(37) "this testcase test strtok() function "
-- Iteration 2 --
string(37) "this testcase test strtok() function "
-- Iteration 3 --
string(37) "this testcase test strtok() function "
-- Iteration 4 --
string(37) "this testcase test strtok() function "
-- Iteration 5 --
string(37) "this testcase test strtok() function "
-- Iteration 6 --
string(37) "this testcase test strtok() function "
-- Iteration 7 --
string(37) "this testcase test strtok() function "
-- Iteration 8 --
string(37) "this testcase test strtok() function "
-- Iteration 9 --
string(37) "this testcase test strtok() function "
-- Iteration 10 --

Warning: strtok() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: strtok() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: strtok() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: strtok() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: strtok() expects parameter 2 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
string(37) "this testcase test strtok() function "
-- Iteration 16 --
string(37) "this testcase test strtok() function "
-- Iteration 17 --
string(37) "this testcase test strtok() function "
-- Iteration 18 --
string(37) "this testcase test strtok() function "
-- Iteration 19 --
string(4) "his "
-- Iteration 20 --
string(37) "this testcase test strtok() function "
-- Iteration 21 --
string(37) "this testcase test strtok() function "
-- Iteration 22 --
string(37) "this testcase test strtok() function "
-- Iteration 23 --
string(37) "this testcase test strtok() function "
-- Iteration 24 --
string(37) "this testcase test strtok() function "
-- Iteration 25 --
string(37) "this testcase test strtok() function "
-- Iteration 26 --

Warning: strtok() expects parameter 2 to be string, resource given in %s on line %d
NULL
Done
