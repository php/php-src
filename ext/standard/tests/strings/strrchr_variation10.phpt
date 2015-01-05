--TEST--
Test strrchr() function : usage variations - unexpected inputs for needle 
--FILE--
<?php
/* Prototype  : string strrchr(string $haystack, string $needle);
 * Description: Finds the last occurrence of a character in a string.
 * Source code: ext/standard/string.c
*/

/* Test strrchr() function: with unexpected inputs for needle 
 *  and expected type for haystack 
*/

echo "*** Testing strrchr() function with unexpected inputs for needle ***\n";

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

$haystacks =  array (
  //integer numeric strings
  "0",
  "1",
  "2",
  "-2",

  //float numeric strings
  "10.5",
  "-10.5",
  "10.5e10",
  "10.6E-10",
  ".5",

  //regular strings
  "array",
  "a",
  "r",
  "y",
  "ay",
  "true",
  "false",
  "TRUE",
  "FALSE",
  "NULL",
  "null",
  "object",

  //empty string
  "",
  '',

  //resource variable in string form
  "\$file_handle",

  //undefined variable in string form
  @"$undefined_var",
  @"$unset_var"
);

// array with different values
$needles =  array (

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

  // null vlaues
  NULL,
  null,

  // objects
  new sample(),

  // empty string
  "",
  '',

  // resource
  $file_handle,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);

// loop through each element of the array and check the working of strrchr()
$count = 1;
for($index = 0; $index < count($haystacks); $index++) {
  echo "-- Iteration $count --\n";
  var_dump( strrchr($haystacks[$index], $needles[$index]) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrchr() function with unexpected inputs for needle ***
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

Warning: strrchr(): needle is not a string or an integer in %s on line %d
bool(false)
-- Iteration 11 --

Warning: strrchr(): needle is not a string or an integer in %s on line %d
bool(false)
-- Iteration 12 --

Warning: strrchr(): needle is not a string or an integer in %s on line %d
bool(false)
-- Iteration 13 --

Warning: strrchr(): needle is not a string or an integer in %s on line %d
bool(false)
-- Iteration 14 --

Warning: strrchr(): needle is not a string or an integer in %s on line %d
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

Notice: Object of class sample could not be converted to int in %s on line %d
bool(false)
-- Iteration 22 --
bool(false)
-- Iteration 23 --
bool(false)
-- Iteration 24 --

Warning: strrchr(): needle is not a string or an integer in %s on line %d
bool(false)
-- Iteration 25 --
bool(false)
-- Iteration 26 --
bool(false)
*** Done ***
