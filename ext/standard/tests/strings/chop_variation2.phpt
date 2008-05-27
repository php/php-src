--TEST--
Test chop() function : usage variations  - unexpected values for charlist argument
--FILE--
<?php
/* Prototype  : string chop ( string $str [, string $charlist] )
 * Description: Strip whitespace (or other characters) from the end of a string
 * Source code: ext/standard/string.c
*/

/*
 * Testing chop() : with different unexpected values of charlist argument passed tot he function
*/

echo "*** Testing chop() : with unexpected values of charlist argument passed to the function ***\n";
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

  // null vlaues
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
// when $charlist arugment is supplied with different values
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
*** Testing chop() : with unexpected values of charlist argument passed to the function ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing chop() by supplying different values for 'charlist' argument ---
-- Iteration 1 --
unicode(17) "hello world12345 "
-- Iteration 2 --
unicode(17) "hello world12345 "
-- Iteration 3 --
unicode(17) "hello world12345 "
-- Iteration 4 --
unicode(17) "hello world12345 "
-- Iteration 5 --
unicode(17) "hello world12345 "
-- Iteration 6 --
unicode(17) "hello world12345 "
-- Iteration 7 --
unicode(17) "hello world12345 "
-- Iteration 8 --
unicode(17) "hello world12345 "
-- Iteration 9 --
unicode(17) "hello world12345 "
-- Iteration 10 --

Warning: chop() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: chop() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --

Warning: chop() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 13 --

Warning: chop() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 14 --

Warning: chop() expects parameter 2 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 15 --
unicode(17) "hello world12345 "
-- Iteration 16 --
unicode(17) "hello world12345 "
-- Iteration 17 --
unicode(17) "hello world12345 "
-- Iteration 18 --
unicode(17) "hello world12345 "
-- Iteration 19 --
unicode(11) "hello world"
-- Iteration 20 --
unicode(17) "hello world12345 "
-- Iteration 21 --
unicode(17) "hello world12345 "
-- Iteration 22 --
unicode(17) "hello world12345 "
-- Iteration 23 --
unicode(17) "hello world12345 "
-- Iteration 24 --

Warning: chop() expects parameter 2 to be string (Unicode or binary), resource given in %s on line %d
NULL
-- Iteration 25 --
unicode(17) "hello world12345 "
-- Iteration 26 --
unicode(17) "hello world12345 "
Done
