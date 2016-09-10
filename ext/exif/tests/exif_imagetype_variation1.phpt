--TEST--
Test exif_imagetype() function : usage variations  - different types for filename argument
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php

/* Prototype  : int exif_imagetype  ( string $filename  )
 * Description: Determine the type of an image
 * Source code: ext/exif/exif.c
*/

echo "*** Testing exif_imagetype() : different types for filename argument ***\n";
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


// loop through each element of the array and check the working of exif_imagetype()
// when $filename is supplied with different values

echo "\n--- Testing exif_imagetype() by supplying different values for 'filename' argument ---\n";
$counter = 1;
foreach($values as $filename) {
  echo "-- Iteration $counter --\n";
  var_dump( exif_imagetype($filename) );
  $counter ++;
}

// closing the file
fclose($file_handle);

echo "Done\n";
?>

?>
===Done===
--EXPECTF--
*** Testing exif_imagetype() : different types for filename argument ***

Notice: Undefined variable: undefined_var in %s on line %d

Notice: Undefined variable: unset_var in %s on line %d

--- Testing exif_imagetype() by supplying different values for 'filename' argument ---
-- Iteration 1 --

Warning: exif_imagetype(0): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 2 --

Warning: exif_imagetype(1): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 3 --

Warning: exif_imagetype(12345): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 4 --

Warning: exif_imagetype(-2345): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 5 --

Warning: exif_imagetype(10.5): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 6 --

Warning: exif_imagetype(-10.5): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 7 --

Warning: exif_imagetype(101234567000): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 8 --

Warning: exif_imagetype(1.07654321E-9): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 9 --

Warning: exif_imagetype(0.5): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 10 --

Warning: exif_imagetype() expects parameter 1 to be a valid path, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: exif_imagetype() expects parameter 1 to be a valid path, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: exif_imagetype() expects parameter 1 to be a valid path, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: exif_imagetype() expects parameter 1 to be a valid path, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: exif_imagetype() expects parameter 1 to be a valid path, array given in %s on line %d
NULL
-- Iteration 15 --

Warning: exif_imagetype(1): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 16 --

Warning: exif_imagetype(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 17 --

Warning: exif_imagetype(1): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 18 --

Warning: exif_imagetype(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 19 --

Warning: exif_imagetype(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 20 --

Warning: exif_imagetype(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 21 --

Warning: exif_imagetype(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 22 --

Warning: exif_imagetype(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 23 --

Warning: exif_imagetype(obj'ct): failed to open stream: No such file or directory in %s on line %d
bool(false)
-- Iteration 24 --

Warning: exif_imagetype() expects parameter 1 to be a valid path, resource given in %s on line %d
NULL
-- Iteration 25 --

Warning: exif_imagetype(): Filename cannot be empty in %s on line %d
bool(false)
-- Iteration 26 --

Warning: exif_imagetype(): Filename cannot be empty in %s on line %d
bool(false)
Done

?>
===Done===
