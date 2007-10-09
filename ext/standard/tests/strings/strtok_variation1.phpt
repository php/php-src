--TEST--
Test strtok() function : usage variations - first argument as non-string
--FILE--
<?php
/* Prototype  : string strtok ( string $str, string $token )
 * Description: splits a string (str) into smaller strings (tokens), with each token being delimited by any character from token
 * Source code: ext/standard/string.c
*/

/*
 * Testing strtok() : with first argument as non-string
*/

echo "*** Testing strtok() : with first argument as non-string ***\n";
// initialize all required variables
$token = '-';

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
  array('color' => 'red-color', 'item' => 'pen-color'),

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
// when $str arugment is supplied with different values

echo "\n--- Testing strtok() by supplying different values for 'str' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $str = $values [$index];

  var_dump( strtok($str, $token) );

  $counter ++;
}

//closing the resource 
fclose($file_handle);

echo "Done\n";
?>
--EXPECTF--
*** Testing strtok() : with first argument as non-string ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing strtok() by supplying different values for 'str' argument ---
-- Iteration 1 --
string(1) "0"
-- Iteration 2 --
string(1) "1"
-- Iteration 3 --
string(5) "12345"
-- Iteration 4 --
string(4) "2345"
-- Iteration 5 --
string(4) "10.5"
-- Iteration 6 --
string(4) "10.5"
-- Iteration 7 --
string(12) "105000000000"
-- Iteration 8 --
string(5) "1.06E"
-- Iteration 9 --
string(3) "0.5"
-- Iteration 10 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 11 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 12 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 13 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 14 --

Notice: Array to string conversion in %s on line %d
string(5) "Array"
-- Iteration 15 --
string(1) "1"
-- Iteration 16 --
bool(false)
-- Iteration 17 --
string(1) "1"
-- Iteration 18 --
bool(false)
-- Iteration 19 --
string(3) "obj"
-- Iteration 20 --
bool(false)
-- Iteration 21 --
bool(false)
-- Iteration 22 --
bool(false)
-- Iteration 23 --
bool(false)
-- Iteration 24 --
bool(false)
-- Iteration 25 --
bool(false)
-- Iteration 26 --
string(%d) "Resource id #%d"
Done
