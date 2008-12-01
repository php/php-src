--TEST--
Test exif_tagname() function : usage variations  - different types for index argument
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php

/* Prototype  : string exif_tagname ( string $index  )
 * Description: Get the header name for an index
 * Source code: ext/exif/exif.c
*/

echo "*** Testing exif_tagname() : different types for index argument ***\n";
// initialize all required variables

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// declaring a class
class sample  {
  public function __toString() {
  return "obj'ct";
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

  // undefined variable
  $undefined_var,

  // unset variable
  $unset_var,
  
  // objects
  new sample(),

  // resource
  $file_handle,
 
  NULL,
  null
);


// loop through each element of the array and check the working of exif_tagname()
// when $index arugment is supplied with different values

echo "\n--- Testing exif_tagname() by supplying different values for 'index' argument ---\n";
$counter = 1;
foreach($values as $index) {
  echo "-- Iteration $counter --\n";
  var_dump( exif_tagname($index) );
  $counter ++;
}

// closing the file
fclose($file_handle);

echo "Done\n";
?>

?>
===Done===
--EXPECTF--
*** Testing exif_tagname() : different types for index argument ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing exif_tagname() by supplying different values for 'index' argument ---
-- Iteration 1 --
bool(false)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(false)
-- Iteration 4 --
bool(false)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(false)
-- Iteration 7 --
bool(false)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
bool(false)
-- Iteration 10 --
bool(false)
-- Iteration 11 --
bool(false)
-- Iteration 12 --
bool(false)
-- Iteration 13 --
bool(false)
-- Iteration 14 --
bool(false)
-- Iteration 15 --
bool(false)
-- Iteration 16 --
bool(false)
-- Iteration 17 --
bool(false)
-- Iteration 18 --
bool(false)
-- Iteration 19 --
bool(false)
-- Iteration 20 --
bool(false)
-- Iteration 21 --
bool(false)
-- Iteration 22 --
bool(false)
-- Iteration 23 --

Notice: Object of class sample could not be converted to int in %s on line %d
bool(false)
-- Iteration 24 --
%s
-- Iteration 25 --
bool(false)
-- Iteration 26 --
bool(false)
Done

?>
===Done===
