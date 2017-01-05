--TEST--
Test stripos() function : usage variations - unexpected inputs for 'haystack', 'needle' & 'offset' arguments
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

/* Test stripos() function with unexpected inputs for 'haystack', 'needle' & 'offset' arguments */

echo "*** Testing stripos() function with unexpected values for haystack, needle & offset ***\n";

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

  //resource
  $file_handle,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);


// loop through each element of the array and check the working of stripos()
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  var_dump( stripos($values[$index], $values[$index], $values[$index]) );
  $counter ++;
}

echo "*** Done ***";
?>
--EXPECTF--
*** Testing stripos() function with unexpected values for haystack, needle & offset ***
-- Iteration 1 --
bool(false)
-- Iteration 2 --
bool(false)
-- Iteration 3 --

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)
-- Iteration 4 --

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)
-- Iteration 5 --

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)
-- Iteration 6 --

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)
-- Iteration 7 --

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
bool(false)
-- Iteration 10 --

Warning: stripos() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: stripos() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: stripos() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: stripos() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: stripos() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
bool(false)
-- Iteration 16 --
bool(false)
-- Iteration 17 --
bool(false)
-- Iteration 18 --
bool(false)
-- Iteration 19 --

Warning: stripos() expects parameter 3 to be integer, object given in %s on line %d
NULL
-- Iteration 20 --

Warning: stripos() expects parameter 3 to be integer, string given in %s on line %d
NULL
-- Iteration 21 --

Warning: stripos() expects parameter 3 to be integer, string given in %s on line %d
NULL
-- Iteration 22 --
bool(false)
-- Iteration 23 --
bool(false)
-- Iteration 24 --

Warning: stripos() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 25 --
bool(false)
-- Iteration 26 --
bool(false)
*** Done ***
