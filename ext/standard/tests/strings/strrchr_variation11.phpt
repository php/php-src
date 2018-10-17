--TEST--
Test strrchr() function : usage variations - unexpected inputs for haystack and needle
--FILE--
<?php
/* Prototype  : string strrchr(string $haystack, string $needle);
 * Description: Finds the last occurrence of a character in a string.
 * Source code: ext/standard/string.c
*/

/* Test strrchr() function with unexpected inputs for haystack and needle */

echo "*** Testing strrchr() function: with unexpected inputs for haystack and needle ***\n";

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// declaring a class
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
  @$undefined_var,

  // unset variable
  @$unset_var
);


// loop through each element of the array and check the working of strrchr()
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  var_dump( strrchr($values[$index], $values[$index]) );
  $counter ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrchr() function: with unexpected inputs for haystack and needle ***
-- Iteration 1 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 2 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 3 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 4 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 5 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 6 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 7 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 8 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 9 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 10 --

Warning: strrchr() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: strrchr() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: strrchr() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: strrchr() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: strrchr() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 15 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 16 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 17 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 18 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 19 --

Notice: Object of class sample could not be converted to int in %s on line %d

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 20 --
bool(false)
-- Iteration 21 --
bool(false)
-- Iteration 22 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 23 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 24 --

Warning: strrchr() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 25 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
-- Iteration 26 --

Deprecated: strrchr(): Non-string needles will be interpreted as strings in %s on line %d
bool(false)
*** Done ***
