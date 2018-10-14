--TEST--
Test strrpos() function : usage variations - unexpected inputs for 'offset' argument
--SKIPIF--
<?php if (PHP_INT_SIZE !== 4) die("skip this test is for 32-bit only");
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'.
 * Source code: ext/standard/string.c
*/

/* Test strrpos() function with unexpected inputs for 'offset' argument */

echo "*** Testing strrpos() function: with unexpected values for offset ***\n";

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// defining a class
class sample  {
  public function __toString() {
    return "object";
  }
}

//getting the resource
$file_handle = fopen(__FILE__, "r");

//definition of input args
$haystack = "hello world";
$needle = "world";

// array with different values
$offsets =  array (

  // float values
  1.5,
  -1.5,
  1.5e10,
  1.6E-10,
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

  //resource
  $file_handle,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);


// loop through each element of the array and check the working of strrpos()
$counter = 1;
for($index = 0; $index < count($offsets); $index ++) {
  echo "-- Iteration $counter --\n";
  var_dump( strrpos($haystack, $needle, $offsets[$index]) );
  $counter ++;
}

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrpos() function: with unexpected values for offset ***
-- Iteration 1 --
int(6)
-- Iteration 2 --
int(6)
-- Iteration 3 --

Warning: strrpos() expects parameter 3 to be int, float given in %s on line %d
bool(false)
-- Iteration 4 --
int(6)
-- Iteration 5 --
int(6)
-- Iteration 6 --

Warning: strrpos() expects parameter 3 to be int, array given in %s on line %d
bool(false)
-- Iteration 7 --

Warning: strrpos() expects parameter 3 to be int, array given in %s on line %d
bool(false)
-- Iteration 8 --

Warning: strrpos() expects parameter 3 to be int, array given in %s on line %d
bool(false)
-- Iteration 9 --

Warning: strrpos() expects parameter 3 to be int, array given in %s on line %d
bool(false)
-- Iteration 10 --

Warning: strrpos() expects parameter 3 to be int, array given in %s on line %d
bool(false)
-- Iteration 11 --
int(6)
-- Iteration 12 --
int(6)
-- Iteration 13 --
int(6)
-- Iteration 14 --
int(6)
-- Iteration 15 --

Warning: strrpos() expects parameter 3 to be int, object given in %s on line %d
bool(false)
-- Iteration 16 --

Warning: strrpos() expects parameter 3 to be int, string given in %s on line %d
bool(false)
-- Iteration 17 --

Warning: strrpos() expects parameter 3 to be int, string given in %s on line %d
bool(false)
-- Iteration 18 --
int(6)
-- Iteration 19 --
int(6)
-- Iteration 20 --

Warning: strrpos() expects parameter 3 to be int, resource given in %s on line %d
bool(false)
-- Iteration 21 --
int(6)
-- Iteration 22 --
int(6)
*** Done ***
